#pragma once
#include <string>
#include <sstream>
#include <iostream>

using ::std::string;

class LogItem
{
	private:
		char var[3];
		short address;
		char type[5];
		string label;
		double value = 0.0;
		short size = 1;
	public:
		LogItem(char *, short, char *, string);
		char* getVar();
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

