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

string LogItem::getLabel()
{
	return label;
}

void LogItem::setValue(short value)
{
	this->value = value;
}

short LogItem::getValue()
{
	return value;
}
