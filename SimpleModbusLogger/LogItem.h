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
		char type[6];
		short size = 1;
		string label;
		double value = 0.0;
		static const std::map<string, short> ITEM_TYPE_SIZES;
		bool isReal();
		bool isLReal();
		bool isSigned();
		struct TypeUnions;
	public:
		class TypeException;
		LogItem(char *, short, const char *, string);
		char* getVar();
		short getAddress();
		void setSize(short size);
		short getSize();
		string getLabel();
		template <class T>
		void setValue(T &value);
		double getValue();
		static const char* ITEM_INPUT;
		static const char* ITEM_OUTPUT;
		static const char* ITEM_REGISTER;
		static const char* ITEM_ANALOG_INPUT;
};

template <class T>
void LogItem::setValue(T& value)
{
	//Jeœli zmienna nie jest rejestrem
	if (strcmp(this->var, ITEM_REGISTER))
	{
		this->value = value;	//< Zapisz wartoœæ
		return;
	}
	
	//Jeœli zmienna jest rejestrem, trzeba obs³u¿yæ jej typ

	unsigned long long temp = 0;

	T *value_ptr = &value;

	for (int i = 0; i < size; i++)
	{
		temp <<= 16;
		temp |= *value_ptr;
		value_ptr++;
	}

	std::cout << "temp: " << temp << std::endl;

	if (isReal())
	{
		union TypeUnions::u_float f;
		f.bits = temp;
		this->value = f.value;
	}
	else if (isLReal())
	{
		union TypeUnions::u_double d;
		d.bits = temp;
		this->value = d.value;
	}
	else if (isSigned())
	{
		bool sign = (temp >> (16 * size - 1)) & 1;
		if (sign)
		{
			unsigned long long ones = -1;
			ones <<= (16 * size);
			temp |= ones;
		}
		this->value = (long long) temp;
	}
	else
		this->value = temp;

	std::cout << "value: " << this->value << std::endl;
}

class LogItem::TypeException : public std::exception
{
	private:
		string type;
	public:
		TypeException(string type);
		const char* what();
};

struct LogItem::TypeUnions
{
	union u_float
	{
		float value;
		unsigned long long bits;
	};
	union u_double
	{
		double value;
		unsigned long long bits;
	};
};