#include "LogItem.h"

const char* LogItem::ITEM_INPUT = "I";
const char* LogItem::ITEM_OUTPUT = "Q";
const char* LogItem::ITEM_REGISTER = "R";
const char* LogItem::ITEM_ANALOG_INPUT = "AI";

LogItem::LogItem(char *type, short address, string label)
{
	strcpy_s(this->type, 3, type);
	this->address = address;
	this->label = label;
}

char* LogItem::getType()
{
	return type;
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
