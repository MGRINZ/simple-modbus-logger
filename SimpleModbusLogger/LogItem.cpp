#include "LogItem.h"

const char* LogItem::ITEM_INPUT = "I";
const char* LogItem::ITEM_OUTPUT = "Q";
const char* LogItem::ITEM_REGISTER = "R";
const char* LogItem::ITEM_ANALOG_INPUT = "AI";

const std::map<string, short> LogItem::ITEM_TYPE_SIZES = {
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
bool LogItem::isReal()
{
	return !strcmp(type, "REAL");
}

///
///Sprawdza czy pobierana zmienna jest typu LREAL
///
bool LogItem::isLReal()
{
	return !strcmp(type, "LREAL");
}

///
///Sprawdza czy pobierana zmienna jest typu ze znakiem
///
bool LogItem::isSigned()
{
	return type[0] != 'U';
}

LogItem::LogItem(char *var, short address, const char *type, string label)
{
	strcpy_s(this->var, sizeof(this->var), var);
	this->address = address;

	if (!strlen(type))
		strcpy_s(this->type, sizeof(this->type), "INT");
	else
		strcpy_s(this->type, sizeof(this->type), type);
	
	this->label = label;

	//Ustalenie rozmiaru zmiennej
	if (ITEM_TYPE_SIZES.count(this->type))
		size = ITEM_TYPE_SIZES.at(this->type);
	else
		throw TypeException(this->type);
}

char* LogItem::getVar()
{
	return var;
}

short LogItem::getAddress()
{
	return address;
}

void LogItem::setSize(short size)
{
	this->size = size;
}

short LogItem::getSize()
{
	return this->size;
}

string LogItem::getLabel()
{
	return label;
}

double LogItem::getValue()
{
	return value;
}

LogItem::TypeException::TypeException(string type)
{
	this->type = type;
}

const char* LogItem::TypeException::what()
{
	string msg = "Unknown type " + type;
	return msg.c_str();
}
