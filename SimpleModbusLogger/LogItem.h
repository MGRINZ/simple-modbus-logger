#pragma once
#include <string>
#include <sstream>
#include <iostream>

using ::std::string;

class LogItem
{
	private:
		char type[3];
		short address;
		string label;
		short value = 0;
		short size = 1;
	public:
		LogItem(char *, short, string);
		char* getType();
		short getAddress();
		void setSize(short size);
		short getSize();
		string getLabel();
		void setValue(uint16_t value);
		double getValue();
		static const char* ITEM_INPUT;
		static const char* ITEM_OUTPUT;
		static const char* ITEM_REGISTER;
		static const char* ITEM_ANALOG_INPUT;
};

