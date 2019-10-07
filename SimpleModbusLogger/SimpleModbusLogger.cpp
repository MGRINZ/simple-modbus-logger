#include <iostream>
#include <modbus.h>
#include <thread>
#include <chrono>
#include "IniParser.h"

void logging_thread(modbus_t *ctx, int input_address)
{
	uint16_t response[1];

	while (true) {
		modbus_read_registers(ctx, input_address, 1, response);
		//modbus_read_input_bits(ctx, input_address, 1, response);
		std::cout << "Input address: " << input_address << " Value: " << response[0] << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

int main()
{
	modbus_t* ctx = nullptr;
	//IniParser config("config.ini");
	//config.get("dsa");

	char connection_type;

	char device[64];
	int baud;
	char parity;
	short data_bit;
	short stop_bit;
	short slave;

	char ip_address[64];
	short port;

	std::cout << "Connection type"
		<< std::endl << "  S - serial"
		<< std::endl << "  T - TCP/IP"
		<< std::endl << "> ";
	std::cin >> connection_type;

	switch (connection_type) {
		case 'S':
		{
			std::cout << "Device: ";
			std::cin >> device;
			std::cout << "Baud: ";
			std::cin >> baud;
			std::cout << "Parity"
				<< std::endl << "  N - none"
				<< std::endl << "  O - odd"
				<< std::endl << "  E - even"
				<< std::endl << "> ";
			std::cin >> parity;
			std::cout << "Data bit: ";
			std::cin >> data_bit;
			std::cout << "Stop bit: ";
			std::cin >> stop_bit;
			std::cout << "Slave: ";
			std::cin >> slave;
			break;
		}
		case 'T':
		{
			std::cout << "IP Address: ";
			std::cin >> ip_address;
			std::cout << "Port: ";
			std::cin >> port;
			break;
		}
		default: 
			return -1;
	}

	if (connection_type == 'S')
	{
		ctx = modbus_new_rtu(device, baud, parity, data_bit, stop_bit);
		modbus_set_slave(ctx, slave);
	}
	else if(connection_type == 'T')
		ctx = modbus_new_tcp(ip_address, port);



	if (modbus_connect(ctx) == -1)
	{
		std::cout << "MODBUS connection error: " << modbus_strerror(errno);
		modbus_flush(ctx);
		return -1;
	}

	std::cout << "Connected" << std::endl;

	int input_address;

	std::cout << "Input address: ";
	std::cin >> input_address;

	std::thread logger(logging_thread, ctx, input_address);

	logger.detach();

	while (true);
}