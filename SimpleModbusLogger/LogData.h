#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <modbus.h>
#include "LogItem.h"

using ::std::string;
using ::std::vector;

class LogData
{
	private:
		string path;
		modbus_t *modbus_ctx;
		vector<LogItem> outputs;
		vector<LogItem> inputs;
		vector<LogItem> registers;
		vector<LogItem> analogInputs;
		
	public:
		LogData(string path, modbus_t *modbus_ctx);
		short minimum(vector<LogItem> &items, int start, int end);
		void sort(vector<LogItem> &items);
		void optimize();
		void log();
		void fetchOutput(short start, short count, vector<LogItem> &items, int item_index);
		void fetchInput(short start, short count, vector<LogItem> &items, int item_index);
		void fetchRegisters(short start, short count, vector<LogItem> &items, int item_index);
		void fetchAnalogInput(short start, short count, vector<LogItem> &items, int item_index);
};

