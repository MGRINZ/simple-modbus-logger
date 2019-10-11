#pragma once
#include <string>

using ::std::string;

class LogItem
{
	private:
		char type[3];
		short address;
		string label;
	public:
		LogItem(char *, short, string);
		char* getType();
		short getAddress();
		string getLabel();
		static const char* ITEM_INPUT;
		static const char* ITEM_OUTPUT;
		static const char* ITEM_REGISTER;
		static const char* ITEM_ANALOG_INPUT;
};

