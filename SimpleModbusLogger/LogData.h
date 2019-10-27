#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <modbus.h>
#include <mysqlx/xdevapi.h>
#include "PlcVariable.h"

using ::std::string;
using ::std::vector;

class LogData
{
	private:
		string path;
		modbus_t *modbus_ctx;
		mysqlx::Session *db;
		short offset = 1;
		vector<PlcVariable> outputs;
		vector<PlcVariable> inputs;
		vector<PlcVariable> registers;
		vector<PlcVariable> analogInputs;
		void save(vector<PlcVariable> &items);
	public:
		LogData(string path, modbus_t *modbus_ctx, mysqlx::Session* db, short offset);
		short minimum(vector<PlcVariable> &items, int start, int end);
		void sort(vector<PlcVariable> &items);
		void optimize();
		void log();
		void fetchOutput(short start, short count, vector<PlcVariable> &items, int item_index);
		void fetchInput(short start, short count, vector<PlcVariable> &items, int item_index);
		void fetchRegisters(short start, short count, vector<PlcVariable> &items, int item_index);
		void fetchAnalogInput(short start, short count, vector<PlcVariable> &items, int item_index);
};

