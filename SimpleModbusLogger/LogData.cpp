#include "LogData.h"

/// 
/// Zapis pobranych ze sterownika zmiennych do bazy danych
///
/// @params	items	wektor zawieraj¹cy listê zmiennych
/// 
void LogData::save(vector<PlcVariable>& items)
{
	for (auto item : items)
	{
		try {
			mysqlx::SqlStatement stmt = db->sql(
				"INSERT INTO data(var, address, type, value, label) VALUES (?, ?, ?, ?, ?)"
			);
			stmt.bind(item.getVar());
			stmt.bind(item.getAddress() + this->offset);
			stmt.bind(item.getType());
			stmt.bind(item.getRealValue());
			stmt.bind(item.getLabel());
			mysqlx::SqlResult result = stmt.execute();
		}
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}

/// 
/// Konstruktor dziennika wartoœci zmiennych
///
/// @param	path		œcie¿ka do pliku zawieraj¹cego listê zmiennych do wczytania
/// @param	modbus_ctx	wskaŸnik do po³¹czenia MODBUS
/// @param	db			wskaŸnik do obiektu po³¹czenia z baz¹ danych MySQL
/// @param	offset		przesuniêcie adresu w sterowniku
/// 
LogData::LogData(string path, modbus_t *modbus_ctx, mysqlx::Session *db, short offset)
{
	this->path = path;
	this->modbus_ctx = modbus_ctx;
	this->db = db;
	this->offset = offset;
}

/// 
/// Obliczanie indeksu najmniejszego adresu na liœcie zmiennych przeznaczonych do zapisu
///
/// @param	items	wektor zawieraj¹cy listê zmiennych
///	@param	start	pocz¹tkowy indeks
///	@param	end		koñcowy indeks
///	@return			indeks najmniejszego adresu na liœcie zmiennych
/// 
short LogData::minimum(vector<PlcVariable> &items, int start, int end)
{
	short min_index = start;

	for (int i = start + 1; i < end; i++)
	{
		if (items[min_index].getAddress() > items[i].getAddress())
			min_index = i;
	}
	return min_index;
}

/// 
/// Sortowanie listy zmiennych przeznaczonych do zapisu
///
/// @param	items	wektor zawieraj¹cy listê zmiennych
/// 
void LogData::sort(vector<PlcVariable> &items)
{
	int items_length = items.size();
	int min = 0;
	for (int i = 0; i < items_length; i++)
	{
		min = minimum(items, i, items_length);
		PlcVariable temp = items[i];
		items[i] = items[min];
		items[min] = temp;
	}
}

/// 
/// Wczytanie listy zmiennych i optymalizacja przez sortowanie
/// Metoda wczytuje listê zmiennych z pliku i tworzy listy zmiennych wed³ug ich rodzajów.
/// Nastêpnie wywo³uje metodê sortowania, aby w ramach pojedynczego zapytania mo¿liwe
/// by³o pobranie wielu wartoœci zmiennych, co zapewnia krótszy czas dzia³ania programu.
/// 
void LogData::optimize()
{
	std::ifstream data_file(path);
	string item_str;

	char var[3];
	int address = 0;
	char type[6];
	string label;

	int semicolon = 0;

	inputs.clear();
	outputs.clear();
	registers.clear();
	analogInputs.clear();

	while (std::getline(data_file, item_str))
	{
		std::stringstream line(item_str);
		string token;
		
		std::getline(line, token, ';');
		
		if (token[0] == '#')
			continue;
		
		try {
			var[0] = token[1];

			if (!isdigit(token[2]))
			{
				var[1] = token[2];
				var[2] = '\0';
			}
			else
				var[1] = '\0';

			address = atoi(token.substr(strlen(var) + 1).c_str());

			std::getline(line, token, ';');
			if (token.size() > sizeof(type))
				throw PlcVariable::TypeException(token);
			strcpy_s(type, sizeof(type), token.c_str());
		
			std::getline(line, token, ';');
			label = token;
		
			PlcVariable item(var, address - this->offset, type, label);

			if(!strcmp(var, PlcVariable::ITEM_INPUT))
				inputs.push_back(item);
			else if(!strcmp(var, PlcVariable::ITEM_OUTPUT))
				outputs.push_back(item);
			else if(!strcmp(var, PlcVariable::ITEM_REGISTER))
				registers.push_back(item);
			else if(!strcmp(var, PlcVariable::ITEM_ANALOG_INPUT))
				analogInputs.push_back(item);
		}
		catch (PlcVariable::TypeException& e)
		{
			std::cout << e.what() << std::endl;
		}
		catch (PlcVariable::AddressException& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	
	sort(inputs);
	sort(outputs);
	sort(registers);
	sort(analogInputs);
}

/// 
/// Wczytanie wartoœci zmiennych i zapis do bazy danych
/// Metoda wysy³a kolejne zapytania do sterownika o wartoœci wybranych zmiennych.
/// Na podstawie posortowanych list zmiennych okreœla przedzia³y adresów, które
/// maj¹ zostaæ odczytane. Nastêpnie przekazuje ka¿d¹ pobran¹ zmienn¹ do zapisu do bazy
/// danych.
/// 
void LogData::log()
{
	vector<PlcVariable>* items[4] = {
		&inputs,
		&outputs,
		&registers,
		&analogInputs
	};

	for (int i = 0; i < 4; i++)
	{
		vector<PlcVariable>& item = *items[i];
		int items_length = item.size();

		if (!items_length)
			continue;

		int item_index = 0;
		int start = item[0].getAddress();
		int count = item[0].getSize();
		bool split = false;

		for (int j = 1; j < items_length; j++)
		{
			if (item[j].getAddress() == item[j - 1].getAddress() + item[j - 1].getSize())
				count += item[j].getSize();

			if (count > 100)
			{
				split = true;
				count -= item[j].getSize();
			}

			if (item[j].getAddress() != item[j - 1].getAddress() + item[j - 1].getSize() || j == items_length - 1 || split)
			{
				if (!strcmp(item[0].getVar(), "Q"))
					fetchOutput(start, count, item, item_index);
				else if (!strcmp(item[0].getVar(), "I"))
					fetchInput(start, count, item, item_index);
				else if (!strcmp(item[0].getVar(), "R"))
					fetchRegisters(start, count, item, item_index);
				else if (!strcmp(item[0].getVar(), "AI"))
					fetchAnalogInput(start, count, item, item_index);
				
				item_index = j;
				start = item[item_index].getAddress();
				count = item[item_index].getSize();

				split = false;
			}
		}

		save(item);
	}
}

///
/// Pobranie wartoœci wyjœæ dyskretnych
/// 
/// @param	start		pocz¹tkowy adres
/// @param	count		iloœæ zmiennych
/// @param	items		wektor zawieraj¹cy listê zmiennych
/// @param	item_index	indeks w wektorze, w którym znajduje siê pocz¹tkowy adres
///
void LogData::fetchOutput(short start, short count, vector<PlcVariable>& items, int item_index)
{
	uint8_t* dest = new uint8_t[count];
	modbus_read_bits(modbus_ctx, start, count, dest);

	for (int i = 0; i < count; i++)
		items[item_index + i].setValue(dest[i]);

	delete[] dest;
}

///
/// Pobranie wartoœci wejœæ dyskretnych
/// 
/// @param	start		pocz¹tkowy adres
/// @param	count		iloœæ zmiennych
/// @param	items		wektor zawieraj¹cy listê zmiennych
/// @param	item_index	indeks w wektorze, w którym znajduje siê pocz¹tkowy adres
///
void LogData::fetchInput(short start, short count, vector<PlcVariable>& items, int item_index)
{
	uint8_t* dest = new uint8_t[count];
	modbus_read_input_bits(modbus_ctx, start, count, dest);

	for (int i = 0; i < count; i += items[item_index + i].getSize())
		items[item_index + i].setValue(dest[i]);

	delete[] dest;
}

///
/// Pobranie wartoœci rejestrów
/// 
/// @param	start		pocz¹tkowy adres
/// @param	count		iloœæ zmiennych
/// @param	items		wektor zawieraj¹cy listê zmiennych
/// @param	item_index	indeks w wektorze, w którym znajduje siê pocz¹tkowy adres
///
void LogData::fetchRegisters(short start, short count, vector<PlcVariable> &items, int item_index)
{
	uint16_t *dest = new uint16_t[count];
	modbus_read_registers(modbus_ctx, start, count, dest);

	int item_i = 0;
	int dest_i = 0;
	while (dest_i < count)
	{
		items[item_index + item_i].setValue(dest[dest_i]);
		
		dest_i += items[item_index + item_i].getSize();
		item_i++;
	}

	delete[] dest;
}

///
/// Pobranie wartoœci wejœæ analogowych
/// 
/// @param	start		pocz¹tkowy adres
/// @param	count		iloœæ zmiennych
/// @param	items		wektor zawieraj¹cy listê zmiennych
/// @param	item_index	indeks w wektorze, w którym znajduje siê pocz¹tkowy adres
///
void LogData::fetchAnalogInput(short start, short count, vector<PlcVariable>& items, int item_index)
{
	uint16_t* dest = new uint16_t[count];
	modbus_read_input_registers(modbus_ctx, start, count, dest);

	for (int i = 0; i < count; i++)
		items[item_index + i].setValue(dest[i]);

	delete[] dest;
}
