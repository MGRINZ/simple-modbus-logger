#include "LogData.h"

void LogData::save(vector<LogItem>& items)
{
	for (auto item : items)
	{
		try {
			mysqlx::SqlStatement stmt = db->sql(
				"INSERT INTO data(type, address, value, label) VALUES (?, ?, ?, ?)"
			);
			stmt.bind(item.getType());
			stmt.bind(item.getAddress());
			stmt.bind(item.getValue());
			stmt.bind(item.getLabel());
			stmt.execute();
		}
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}

LogData::LogData(string path, modbus_t *modbus_ctx, mysqlx::Session *db)
{
	this->path = path;
	this->modbus_ctx = modbus_ctx;
	this->db = db;
}

short LogData::minimum(vector<LogItem> &items, int start, int end)
{
	short min_index = start;

	for (int i = start + 1; i < end; i++)
	{
		if (items[min_index].getAddress() > items[i].getAddress())
			min_index = i;
	}
	return min_index;
}

void LogData::sort(vector<LogItem> &items)
{
	int items_length = items.size();
	int min = 0;
	for (int i = 0; i < items_length; i++)
	{
		min = minimum(items, i, items_length);
		LogItem temp = items[i];
		items[i] = items[min];
		items[min] = temp;
	}
}

void LogData::optimize()
{
	std::ifstream data_file(path);
	string item_str;

	char type[3];
	short address = 0;
	string label;

	int semicolon = 0;

	inputs.clear();
	outputs.clear();
	registers.clear();
	analogInputs.clear();

	while (std::getline(data_file, item_str))
	{
		std::cout << "F: " << item_str << std::endl;
		type[0] = item_str[1];
		
		if (!isdigit(item_str[2]))
		{
			type[1] = item_str[2];
			type[2] = '\0';
		}
		else
			type[1] = '\0';

		semicolon = item_str.find(";");

		address = atoi(item_str.substr(strlen(type) + 1, semicolon).c_str());
		
		if (semicolon != -1)
			label = item_str.substr(semicolon + 1);
		else
			label = "";

		LogItem item(type, address, label);

		if(!strcmp(type, LogItem::ITEM_INPUT))
			inputs.push_back(item);
		else if(!strcmp(type, LogItem::ITEM_OUTPUT))
			outputs.push_back(item);
		else if(!strcmp(type, LogItem::ITEM_REGISTER))
			registers.push_back(item);
		else if(!strcmp(type, LogItem::ITEM_ANALOG_INPUT))
			analogInputs.push_back(item);
	}
	
	sort(inputs);
	sort(outputs);
	sort(registers);
	sort(analogInputs);
}

void LogData::log()
{
	vector<LogItem>* items[4] = {
		&inputs,
		&outputs,
		&registers,
		&analogInputs
	};

	for (int i = 0; i < 4; i++)
	{
		vector<LogItem>& item = *items[i];
		int items_length = item.size();

		if (!items_length)
			continue;

		int item_index = 0;
		int start = item[0].getAddress();
		int count = 1;
		bool split = false;

		for (int j = 1; j < items_length; j++)
		{
			if (item[j].getAddress() - item[j - 1].getAddress() == 1)
				count++;

			if (count > 100)
			{
				split = true;
				count--;
			}

			if (item[j].getAddress() - item[j - 1].getAddress() != 1 || j == items_length - 1 || split)
			{
				if (!strcmp(item[0].getType(), "Q"))
					fetchOutput(start, count, item, item_index);
				else if (!strcmp(item[0].getType(), "I"))
					fetchInput(start, count, item, item_index);
				else if (!strcmp(item[0].getType(), "R"))
					fetchRegisters(start, count, item, item_index);
				else if (!strcmp(item[0].getType(), "AI"))
					fetchAnalogInput(start, count, item, item_index);
				
				std::cout << "i: " << item_index << " c: " << count << " s: " << start << " " << start << "-" << start + count - 1 << std::endl;

				start = item[item_index + count].getAddress();
				item_index += count;

				count = 1;

				split = false;
			}
		}

		save(item);
	}
}

void LogData::fetchOutput(short start, short count, vector<LogItem>& items, int item_index)
{
	uint8_t* dest = new uint8_t[count];
	modbus_read_bits(modbus_ctx, start, count, dest);

	for (int i = 0; i < count; i++)
		items[item_index + i].setValue(dest[i]);

	delete[] dest;
}

void LogData::fetchInput(short start, short count, vector<LogItem>& items, int item_index)
{
	uint8_t* dest = new uint8_t[count];
	modbus_read_input_bits(modbus_ctx, start, count, dest);

	for (int i = 0; i < count; i++)
		items[item_index + i].setValue(dest[i]);

	delete[] dest;
}

void LogData::fetchRegisters(short start, short count, vector<LogItem> &items, int item_index)
{
	uint16_t *dest = new uint16_t[count];
	modbus_read_registers(modbus_ctx, start, count, dest);

	for (int i = 0; i < count; i++)
		items[item_index + i].setValue(dest[i]);

	delete[] dest;
}

void LogData::fetchAnalogInput(short start, short count, vector<LogItem>& items, int item_index)
{
	uint16_t* dest = new uint16_t[count];
	modbus_read_input_registers(modbus_ctx, start, count, dest);

	for (int i = 0; i < count; i++)
		items[item_index + i].setValue(dest[i]);

	delete[] dest;
}
