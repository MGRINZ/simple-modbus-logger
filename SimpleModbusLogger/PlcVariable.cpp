#include "PlcVariable.h"

const char* PlcVariable::ITEM_INPUT = "I";
const char* PlcVariable::ITEM_OUTPUT = "Q";
const char* PlcVariable::ITEM_REGISTER = "R";
const char* PlcVariable::ITEM_ANALOG_INPUT = "AI";

const std::map<string, short> PlcVariable::ITEM_TYPE_SIZES = {
	{"BOOL", 1},
	{"INT", 1},
	{"DINT", 2},
	{"LINT", 4},
	{"UINT", 1},
	{"UDINT", 2},
	{"ULINT", 4},
	{"REAL", 2},
	{"LREAL", 4}
};

///
/// Sprawdza czy pobierana zmienna jest typu REAL
///
/// @return		true - zmienna typu REAL; false - zmienna innego typu
///
bool PlcVariable::isReal()
{
	return !strcmp(type, "REAL");
}

///
/// Sprawdza czy pobierana zmienna jest typu LREAL
///
/// @return		true - zmienna typu LREAL; false - zmienna innego typu
///
bool PlcVariable::isLReal()
{
	return !strcmp(type, "LREAL");
}

///
/// Sprawdza czy pobierana zmienna jest typu ze znakiem
///
/// @return		true - zmienna typu ze znakiem; false - zmienna typu bez znaku
///
bool PlcVariable::isSigned()
{
	return type[0] != 'U';
}

/// 
/// Konstruktor zmiennej sterownika
/// 
/// @param	var		rodzaj zmiennej
/// @param	address	adres zmiennej
/// @param	type	typ zmiennej
/// @param	label	etykieta zmiennej
/// 
PlcVariable::PlcVariable(char *var, int address, const char *type, string label)
{
	strcpy_s(this->var, sizeof(this->var), var);
	if (address < 0 || address > 65535)
		throw AddressException(address);
	this->address = address;

	if (!strcmp(var, ITEM_REGISTER))
	{
		if (!strlen(type))
			strcpy_s(this->type, sizeof(this->type), "INT");
		else
			strcpy_s(this->type, sizeof(this->type), type);
	}
	else if (!strcmp(var, ITEM_ANALOG_INPUT))
		strcpy_s(this->type, sizeof(this->type), "INT");
	else
		strcpy_s(this->type, sizeof(this->type), "BOOL");

	
	this->label = label;

	//Ustalenie rozmiaru zmiennej
	if (ITEM_TYPE_SIZES.count(this->type))
		size = ITEM_TYPE_SIZES.at(this->type);
	else
		throw TypeException(this->type);
}

///
/// Pobieranie rodzaju zmiennej
///
/// @return		rodzaj zmiennej
///
char* PlcVariable::getVar()
{
	return var;
}

///
/// Pobieranie adresu zmiennej
///
/// @return		adres zmiennej
///
short PlcVariable::getAddress()
{
	return address;
}

///
/// Pobieranie typu zmiennej
///
/// @return		typ zmiennej
///
char* PlcVariable::getType()
{
	return this->type;
}

///
/// Pobieranie rozmiaru zmiennej
///
/// @return		rozmiar zmiennej
///
short PlcVariable::getSize()
{
	return this->size;
}

///
/// Pobieranie etykiety zmiennej
///
/// @return		etykieta zmiennej
///
string PlcVariable::getLabel()
{
	return label;
}

///
/// Ustawienie rzeczywistej warto�ci zmiennej
/// Metoda pobiera jako argument warto�� zmiennej odczytan� z bazy danych i przetwarza j�
/// do postaci size rejestr�w.
///
///	@param	value	rzeczywista warto�� zmiennej
///
void PlcVariable::setRealValue(double value)
{
	this->realValue = value;
	long long temp = 0;

	if (isReal())
	{
		union TypeUnions::u_float f;
		f.value = value;
		temp = f.bits;
	}
	else if (isLReal())
	{
		union TypeUnions::u_double d;
		d.value = value;
		temp = d.bits;
	}
	else
		temp = value;

	for (int i = size - 1; i >= 0; i--)
	{
		this->value[i] = temp & ((1 << 16) - 1);
		temp >>= 16;
	}
}

///
/// Pobranie wska�nika do tablicy warto�ci rejestr�w,
/// z kt�rych sk�ada si� rzeczywista warto�� zmiennej.
/// Metoda zwraca prawid�owy wska�nik tylko po wywo�aniu metody setRealValue()
///
///	@return		tablica warto�ci 16-bitowych rejestr�w reprezentuj�cych warto�� zmiennej
///
unsigned short* PlcVariable::getValue()
{
	return this->value;
}

///
/// Pobranie rzeczywistej warto�ci zmiennej.
/// Metoda zwraca warto�� przypisan� metod� setRealValue()
/// lub wyznaczon� z warto�ci rejestr�w metod� setValue()
///
/// @return		rzeczywista warto�� zmiennej
///
double PlcVariable::getRealValue()
{
	return realValue;
}

///
/// Konstruktor wyj�tku b��du typu
///
/// @param	type	typ danych przekazany do zmiennej
///
PlcVariable::TypeException::TypeException(string type)
{
	this->type = type;
}

///
/// Pobranie wiadomo�ci o b��dzie typu
///
/// @return		wiadomo�� o b��dzie
///
const char* PlcVariable::TypeException::what()
{
	string msg = "Unknown type " + type;
	return msg.c_str();
}

///
/// Konstruktor wyj�tku b��du adresu
///
/// @param	address	address przekazany do zmiennej
///
PlcVariable::AddressException::AddressException(int address)
{
	this->address = address;
}

///
/// Pobranie wiadomo�ci o b��dzie adresu
///
/// @return		wiadomo�� o b��dzie
///
const char* PlcVariable::AddressException::what()
{
	std::stringstream msg;
	msg << "MODBUS address must be in range [0; 65535]. Passed " << address;
	return msg.str().c_str();
}
