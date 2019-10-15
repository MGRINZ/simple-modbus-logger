#include <iostream>
#include <fstream>
#include <modbus.h>
#include <thread>
#include <chrono>
#include <mysqlx/xdevapi.h>
#include "IniParser.h"
#include "LogData.h"

void logging_thread(LogData *logdata , short sweep_time)
{
	while (true) {
		auto start = std::chrono::system_clock::now();
		
		logdata->log();

		auto end = std::chrono::system_clock::now();
		long elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		
		std::cout << "T: " << elapsed_time << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(sweep_time - elapsed_time));
	}
}

int main()
{
	modbus_t* ctx = nullptr;
	IniParser config("config.ini");
	mysqlx::Session* db = nullptr;
	LogData* logdata = nullptr;

	const char *connection_type = config.getChar("connection_type");

	const char *device;
	int baud;
	char parity;
	short data_bit;
	short stop_bit;
	short slave;

	const char *ip_address;
	short port;

	string db_host = config.get("db_host");
	string db_port = config.get("db_port");
	string db_user = config.get("db_user");
	string db_password = config.get("db_password");
	string db_database = config.get("db_database");

	short sweep_time = 100;

	if (!strcmp(connection_type, "serial"))
	{
		device = config.getChar("device");
		baud = config.getInt("baud");
		parity = config.getChar("parity")[0];
		data_bit = config.getInt("data_bit");
		stop_bit = config.getInt("stop_bit");
		slave = config.getInt("slave");

		ctx = modbus_new_rtu(device, baud, parity, data_bit, stop_bit);
		modbus_set_slave(ctx, slave);
	}
	else if (!strcmp(connection_type, "tcpip"))
	{
		ip_address = config.getChar("ip_address");
		port = config.getInt("port");
		ctx = modbus_new_tcp(ip_address, port);
	}

	if (modbus_connect(ctx) == -1)
	{
		std::cout << "MODBUS connection error: " << modbus_strerror(errno);
		modbus_flush(ctx);
		return -1;
	}

	sweep_time = config.getInt("sweep_time");

	try
	{
		//uri = "mysqlx://db_user:db_password@db_host:db_port/db_database"
		string uri = "mysqlx://" + db_user + ":" + db_password + "@" + db_host + ":" + db_port + "/" + db_database;
		db = new mysqlx::Session(uri);
		mysqlx::Schema schema = db->createSchema("web_scada", true);
		mysqlx::SqlStatement stmt = db->sql(
			"CREATE TABLE IF NOT EXISTS data("
			"	id int AUTO_INCREMENT,"
			"	type varchar(2) NOT NULL,"
			"	address int NOT NULL,"
			"	value int NOT NULL,"
			"	time timestamp DEFAULT CURRENT_TIMESTAMP NOT NULL,"
			"	label varchar(255),"
			"	PRIMARY KEY(id)"
			");"
		);
		mysqlx::SqlResult result = stmt.execute();
	}
	catch(std::exception e)
	{
		std::cout << "MySQL connection error" << std::endl;
		std::cout << e.what() << std::endl;
		return -1; 
	}

	logdata = new LogData("logdata.cfg", ctx, db);
	logdata->optimize();

	std::thread logger(logging_thread, logdata, sweep_time);
	logger.detach();

	string command;

	while (true);

	delete ctx;
	delete db;
	delete logdata;

	return 0;
}