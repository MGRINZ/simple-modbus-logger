#include "IniParser.h"

IniParser::IniParser(const char *path)
{
	this->path = (char*) path;
}

char* IniParser::get(const char* key)
{
	std::ifstream file(path);
	
	std::cout << file.is_open();

	file.close();
	return nullptr;
}
