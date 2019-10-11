#include <iostream>
#include <fstream>
#include <modbus.h>
#include <thread>
#include <chrono>
#include "IniParser.h"
#include "LogData.h"

void logging_thread(modbus_t *ctx, short sweep_time)
{
	while (true) {
		auto start = std::chrono::system_clock::now();
		
		LogData logdata("logdata.cfg");
		logdata.optimize();
		logdata.log();

		auto end = std::chrono::system_clock::now();
		long elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		
		std::cout << elapsed_time << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(sweep_time));
	}
}

int main()
{
	modbus_t* ctx = nullptr;
	IniParser config("config.ini");

	const char *connection_type = config.getChar("connection_type");

	const char *device;
	int baud;
	char parity;
	short data_bit;
	short stop_bit;
	short slave;

	const char *ip_address;
	short port;

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

	std::thread logger(logging_thread, ctx, sweep_time);
	logger.detach();

	while (true);
}