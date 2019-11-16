#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <map>

using ::std::string;

class PlcVariable
{
	private:
		char var[3];				//< Typ zmiennej I, Q, R, AI
		short address;				//< Adres zmiennej
		char type[6];				//< Typ zmiennej
		short size = 1;				//< Rozmiar zmiennej wyra¿ony w rejestrach
		string label;				//< Etykieta
		unsigned short value[4];	//< Wartoœci rejestrów sk³adaj¹cych siê na zmienn¹
		double realValue = 0.0;		//< Rzeczywista przechowywana wartoœæ zmiennej
		bool isReal();
		bool isLReal();
		bool isSigned();
	public:
		PlcVariable(char* var, int address, const char* type, string label);
		char* getVar();
		short getAddress();
		char* getType();
		short getSize();
		string getLabel();
		template <class T>
		void setValue(T &value);
		void setRealValue(double value);
		unsigned short* getValue();
		double getRealValue();
		static const char* ITEM_INPUT;
		static const char* ITEM_OUTPUT;
		static const char* ITEM_REGISTER;
		static const char* ITEM_ANALOG_INPUT;
		static const std::map<string, short> ITEM_TYPE_SIZES;
		class AddressException;
		class TypeException;
		struct TypeUnions;
};

///
/// Ustawienie wartoœci zmiennej
/// Metoda pobiera referencjê do wartoœci zmiennej odczytanej ze sterownika i zamienia j¹
/// na typ double w celu zapisu do bazy danych.
/// Je¿eli typ zmiennej przekracza rozmiar jednego rejestru, metoda odczytuje size kolejnych
/// komórek pamiêci i zapisuje rzeczywist¹ wartoœæ zmiennej.
///
template <class T>
void PlcVariable::setValue(T& value)
{
	//Jeœli zmienna nie jest rejestrem
	if (strcmp(this->var, ITEM_REGISTER))
	{
		this->realValue = value;	//< Zapisz wartoœæ
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

	if (isReal())
	{
		union TypeUnions::u_float f;
		f.bits = temp;
		this->realValue = f.value;
	}
	else if (isLReal())
	{
		union TypeUnions::u_double d;
		d.bits = temp;
		this->realValue = d.value;
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
		this->realValue = (long long) temp;
	}
	else
		this->realValue = temp;
}

//
// Wyj¹tek zwi¹zany z podaniem nieprawid³owego typu zmiennej
//
class PlcVariable::TypeException : public std::exception
{
	private:
		string type;
	public:
		TypeException(string type);
		const char* what();
};

//
// Wyj¹tek zwi¹zany z podaniem nieprawid³owego adresu zmiennej
//
class PlcVariable::AddressException : public std::exception
{
	private:
		int address;
	public:
		AddressException(int address);
		const char* what();
};

//
// Struktura do zamiany ci¹gu bitów na liczbê typu double i odwrotnie
//
struct PlcVariable::TypeUnions
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