#pragma once
#include <iostream>
#include <fstream>

class IniParser
{
	private:
		char *path;
	public:
		IniParser(const char *path);
		char* get(const char *key);
};

