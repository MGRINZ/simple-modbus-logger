#include <iostream>
#include <modbus.h>
#include <thread>
#include <chrono>

void logging_thread(modbus_t *ctx)
{
	uint16_t response[1];

	while (true) {
		modbus_read_input_registers(ctx, 0, 1, response);
		std::cout << response[0] << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

int main()
{
	modbus_t *ctx;
	
	ctx = modbus_new_rtu("COM4", 9600, 'N', 8, 1);
	modbus_set_slave(ctx, 1);

	if (modbus_connect(ctx) == -1)
	{
		std::cout << "MODBUS connection error: " << modbus_strerror(errno);
		modbus_flush(ctx);
		return -1;
	}

	std::thread logger(logging_thread, ctx);

	logger.detach();

	while (true);
}