#pragma once
#include <iostream>
#include <modbus.h>
#include <mysqlx/xdevapi.h>

class DataWriter
{
	private:
		modbus_t* modbus_ctx;
		mysqlx::Session* db;

	public:
		DataWriter(modbus_t* modbus_ctx, mysqlx::Session* db);
		void write();
};

