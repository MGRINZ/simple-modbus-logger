#include "IniParser.h"

IniParser::IniParser(string path)
{
	this->path = path;
}

string IniParser::get(string key)
{
	std::ifstream file(path);

	string key_value;
	int key_pos;
	string value = "";

	while (std::getline(file, key_value))
	{
		key_pos = key_value.find(key);
		if (key_pos == 0)
			value = key_value.substr(key.length() + 1);
	}

	file.close();

	return value;
}

const char* IniParser::getChar(string key)
{
	string value_str = get(key);
	int value_length = value_str.length() + 1;
	char* value = new char[value_length];
    strcpy_s(value, value_length, value_str.c_str());
	return value;
}

int IniParser::getInt(string key)
{
	return atoi(get(key).c_str());
}
