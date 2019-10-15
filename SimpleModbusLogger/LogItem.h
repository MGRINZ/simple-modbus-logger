#pragma once
#include <string>
#include <sstream>

using ::std::string;

class LogItem
{
	private:
		char type[3];
		short address;
		string label;
		short value = 0;
	public:
		LogItem(char *, short, string);
		char* getType();
		short getAddress();
		string getLabel();
		void setValue(short value);
		short getValue();
		static const char* ITEM_INPUT;
		static const char* ITEM_OUTPUT;
		static const char* ITEM_REGISTER;
		static const char* ITEM_ANALOG_INPUT;
};

