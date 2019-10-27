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
///Sprawdza czy pobierana zmienna jest typu REAL
///
bool PlcVariable::isReal()
{
	return !strcmp(type, "REAL");
}

///
///Sprawdza czy pobierana zmienna jest typu LREAL
///
bool PlcVariable::isLReal()
{
	return !strcmp(type, "LREAL");
}

///
///Sprawdza czy pobierana zmienna jest typu ze znakiem
///
bool PlcVariable::isSigned()
{
	return type[0] != 'U';
}

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

char* PlcVariable::getVar()
{
	return var;
}

short PlcVariable::getAddress()
{
	return address;
}

char* PlcVariable::getType()
{
	return this->type;
}

short PlcVariable::getSize()
{
	return this->size;
}

string PlcVariable::getLabel()
{
	return label;
}

///
/// Ustawienie rzeczywistej wartoœci zmiennej
/// Metoda pobiera jako argument wartoœæ zmiennej odczytan¹ z bazy danych i przetwarza j¹
/// do postaci size rejestrów.
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
/// Pobranie wskaŸnika do tablicy wartoœci rejestrów,
/// z których sk³ada siê rzeczywista wartoœæ zmiennej.
/// Metoda zwraca prawid³owy wskaŸnik tylko po wywo³aniu metody setRealValue()
///
unsigned short* PlcVariable::getValue()
{
	return this->value;
}

///
/// Pobranie rzeczywistej wartoœci zmiennej.
/// Metoda zwraca wartoœæ przypisan¹ metod¹ setRealValue()
/// lub wyznaczon¹ z wartoœci rejestrów metod¹ setValue()
///
double PlcVariable::getRealValue()
{
	return realValue;
}

PlcVariable::TypeException::TypeException(string type)
{
	this->type = type;
}

const char* PlcVariable::TypeException::what()
{
	string msg = "Unknown type " + type;
	return msg.c_str();
}

PlcVariable::AddressException::AddressException(int address)
{
	this->address = address;
}

const char* PlcVariable::AddressException::what()
{
	std::stringstream msg;
	msg << "MODBUS address must be in range [0; 65535]. Passed " << address;
	return msg.str().c_str();
}
