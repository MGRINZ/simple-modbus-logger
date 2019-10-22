#include "LogItem.h"

const char* LogItem::ITEM_INPUT = "I";
const char* LogItem::ITEM_OUTPUT = "Q";
const char* LogItem::ITEM_REGISTER = "R";
const char* LogItem::ITEM_ANALOG_INPUT = "AI";

LogItem::LogItem(char *var, short address, char *type, string label)
{
	strcpy_s(this->var, sizeof(this->var), var);
	this->address = address;
	strcpy_s(this->type, sizeof(this->type), type);
	this->label = label;
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
