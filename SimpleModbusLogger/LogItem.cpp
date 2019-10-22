#include "LogItem.h"

const char* LogItem::ITEM_INPUT = "I";
const char* LogItem::ITEM_OUTPUT = "Q";
const char* LogItem::ITEM_REGISTER = "R";
const char* LogItem::ITEM_ANALOG_INPUT = "AI";

const std::map<string, short> LogItem::ITEM_TYPE_SIZES = {
	{"INT", 1},
	{"DINT", 2}
};

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
	size = ITEM_TYPE_SIZES.at(this->type);
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

void LogItem::setValue(uint16_t value)
{
	this->value = value;
}

double LogItem::getValue()
{
	return value;
}
