#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <map>

using ::std::string;

class LogItem
{
	private:
		char var[3];
		short address;
		char type[5];
		short size = 1;
		string label;
		double value = 0.0;
		static const std::map<string, short> ITEM_TYPE_SIZES;
	public:
		LogItem(char *, short, const char *, string);
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

