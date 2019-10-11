#pragma once
#include <iostream>
#include <fstream>
#include <string>

using std::string;

class IniParser
{
	private:
		string path;
	public:
		IniParser(string path);
		string get(string key);
		const char* getChar(string key);
		int getInt(string key);
};

