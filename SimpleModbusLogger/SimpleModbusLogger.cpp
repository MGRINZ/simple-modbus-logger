#include <iostream>
#include <fstream>
#include <modbus.h>
#include <thread>
#include <chrono>
#include <mysqlx/xdevapi.h>
#include "IniParser.h"
#include "LogData.h"
#include "LoggerSettings.h"

void logging_thread(LoggerSettings *settings)
{
	while (true) {
		//Obsługa zatrzymania cyklu
		if (settings->isStopSignalSent())	//< Po otrzymaniu sygnału zatrzymania
		{
			settings->setStopped(true);		//< Ustaw stan na "zatrzymany" 
			break;							//< Zatrzymaj cykl
		}

		//Obsługa wstrzymania cyklu
		if (settings->isPauseSignalSent())	//< Po otrzymaniu sygnału wstrzymania
			settings->setPaused(true);		//< Ustaw stan na "wstrzymany"

		if (settings->isPaused())			//< Jeśli stan == "wstrzymany"
		{
			//Obsługa wznowienia cyklu
			if (settings->isResumeSignalSent())	//< Po otrzymaniu sygnału wznowienia
				settings->setPaused(false);		//< Ustaw stan na "działa"
			else								//< Jeśli dalej wstrzymany
				continue;						//< Pomiń cykl
		}

		//Pobierz czas rozpoczęcia cyklu
		auto start = std::chrono::system_clock::now();
		
		//Zapisz zmienne do sterownika
		settings->getDataWriter()->write();

		//Pobierz dane i zapisz do bazy danych
		//settings->getLogData()->log();

		//Pobierz czas zakończenia cyklu i wyznacz czas trwania cyklu
		auto end = std::chrono::system_clock::now();
		long elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		
		std::cout << "T: " << elapsed_time << std::endl;

		//Opóźnij rozpoczęcie kolejnego cyklu
		std::this_thread::sleep_for(std::chrono::milliseconds(settings->getSweepTime() - elapsed_time));
	}
}

int main()
{
	modbus_t* ctx = nullptr;
	mysqlx::Session* db = nullptr;
	LogData* logdata = nullptr;
	DataWriter* dataWriter = nullptr;
	
	//Wczytanie pliku konfiguracji
	IniParser config("config.ini");

	//Ustawienie typu połączenia (<serial, tcpip>)
	const char *connection_type = config.getChar("connection_type");

	//Ustawienia połączenia szeregowego
	const char *device;
	int baud;
	char parity;
	short data_bit;
	short stop_bit;
	short slave;

	//Ustawienia połączenia TCP/IP
	const char *ip_address;
	short port;

	//Wczytanie ustawień serwera MySQL
	string db_host = config.get("db_host");
	string db_port = config.get("db_port");
	string db_user = config.get("db_user");
	string db_password = config.get("db_password");
	string db_database = config.get("db_database");

	//Domyślny czas trwania cyklu programu
	short sweep_time = 100;

	//Wczytanie ustawień połączenia MODBUS
	if (!strcmp(connection_type, "serial"))
	{
		//Wczytanie ustawień dla połączenia MODBUS RTU
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
		//Wczytanie ustawień dla połączenia MODBUS TCP/IP
		ip_address = config.getChar("ip_address");
		port = config.getInt("port");
		ctx = modbus_new_tcp(ip_address, port);
	}

	//Nawiązanie połączenia MODBUS i sprawdzenie poprawności
	if (modbus_connect(ctx) == -1)
	{
		std::cout << "MODBUS connection error: " << modbus_strerror(errno);
		modbus_flush(ctx);
		return -1;
	}

	try
	{
		//Nawiązanie połączenia z bazą danych MySQL
		//uri = "mysqlx://<db_user>:<db_password>@<db_host>:<db_port>/<db_database>"
		string uri = "mysqlx://" + db_user + ":" + db_password + "@" + db_host + ":" + db_port + "/" + db_database;
		db = new mysqlx::Session(uri);

		//Utworzenie bazy danych, jeśli nie istnieje
		mysqlx::Schema schema = db->createSchema(db_database, true);

		//Utworzenie tabel jeśli nie istnieją		
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

		stmt = db->sql(
			"CREATE TABLE IF NOT EXISTS writes("
			"	id int AUTO_INCREMENT,"
			"	type varchar(2) NOT NULL,"
			"	address int NOT NULL,"
			"	value int NOT NULL,"
			"	PRIMARY KEY(id)"
			");"
		);
		result = stmt.execute();
	}
	catch(std::exception e)
	{
		//Obsługa błędów połączenia
		std::cout << "MySQL connection error" << std::endl;
		std::cout << e.what() << std::endl;
		return -1; 
	}

	//Wczytanie czasu cyklu programu
	sweep_time = config.getInt("sweep_time");

	//Przygotowanie obiektu zapisującego dane do sterownika 
	dataWriter = new DataWriter(ctx, db);

	//Przygotowanie listy zmiennych do odczytu
	logdata = new LogData("logdata.cfg", ctx, db);
	logdata->optimize();
	
	//Utworzenie obiektu do komunikacji z wątkiem dziennika
	LoggerSettings* loggerSettings = new LoggerSettings();
	loggerSettings->setLogData(logdata);
	loggerSettings->setDataWriter(dataWriter);
	loggerSettings->setSweepTime(sweep_time);

	//Utworzenie wątku dziennika
	std::thread logger(logging_thread, loggerSettings);
	logger.detach();

	string command;

	//Pętla głównego wątku, obsługa CLI
	while (true)
	{
		std::cout << "> ";
		std::cin >> command;

		// Zakończenie pracy programu
		if (command == "stop")
		{
			std::cout << "Stopping...";
			loggerSettings->stop();

			//Oczekiwanie na zakończenie ostatniego cyklu
			while (!loggerSettings->isStopped());
			
			//Przerwanie pętli głównego wątku
			break;
		}
		
		//Wstrzymanie pracy programu
		if (command == "pause")
		{
			if (loggerSettings->isPaused())
			{
				std::cout << "Error: Logger is already paused." << std::endl;
				continue;
			}

			std::cout << "Pausing..." << std::endl;

			loggerSettings->pause();

			std::cout << "Paused" << std::endl;

			continue;
		}

		//Wznowienie pracy programu
		if (command == "resume")
		{
			if (!loggerSettings->isPaused())
			{
				std::cout << "Error: Logger is already resumed." << std::endl;
				continue;
			}

			std::cout << "Resuming..." << std::endl;

			loggerSettings->resume();

			std::cout << "Resumed" << std::endl;

			continue;
		}

		//Przeładowanie listy zmiennych do odczytu
		if (command == "reload")
		{
			std::cout << "Pausing..." << std::endl;
			loggerSettings->pause();
			std::cout << "Paused" << std::endl;

			std::cout << "Reloading..." << std::endl;
			logdata->optimize();
			std::cout << "Reloaded..." << std::endl;

			std::cout << "Resuming..." << std::endl;
			loggerSettings->resume();
			std::cout << "Resumed" << std::endl;

			continue;
		}
	}

	//Zwolnienie zasobów
	modbus_free(ctx);
	delete db;
	delete logdata;

	return 0;
}