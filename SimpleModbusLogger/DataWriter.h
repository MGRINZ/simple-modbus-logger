#pragma once
#include <iostream>
#include <string>
#include <modbus.h>
#include <mysqlx/xdevapi.h>
#include "PlcVariable.h"

using ::std::string;

class DataWriter
{
	private:
		modbus_t* modbus_ctx;
		mysqlx::Session* db;

	public:
		DataWriter(modbus_t* modbus_ctx, mysqlx::Session* db);
		void write();
};

