#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "LogItem.h"

using ::std::string;
using ::std::vector;

class LogData
{
	private:
		string path;
		vector<LogItem> outputs;
		vector<LogItem> inputs;
		vector<LogItem> registers;
		vector<LogItem> analogInputs;
		
	public:
		LogData(string path);
		short minimum(vector<LogItem> &items, int start, int end);
		void sort(vector<LogItem> &items);
		void optimize();
		void log();
		void logOutput();
		void logInput();
		void logRegisters();
		void logAnalogInput();
};

