#include "DataWriter.h"

DataWriter::DataWriter(modbus_t* modbus_ctx, mysqlx::Session* db, short offset)
{
	this->modbus_ctx = modbus_ctx;
	this->db = db;
	this->offset = offset;
}

void DataWriter::write()
{
	mysqlx::SqlResult qResult;	//< Wynik zapytania
	mysqlx::SqlResult dResult;	//< Wynik usuwania
	mysqlx::Row row;			//< Pojedynczy rekord

	//Polecenie usuniêcia
	mysqlx::SqlStatement dStmt = db->sql(
		"DELETE FROM writes "
		"WHERE id = ?"
	);
	
	//Zapytanie o wyjœcia
	mysqlx::SqlStatement qStmt = db->sql(
		"SELECT id, address, value FROM writes "
		"WHERE var = 'Q'"
	);
	qResult = qStmt.execute();

	//Zapis wyjœæ dyskretnych
	while (row = qResult.fetchOne()) {
		int address = row[1];
		double value = row[2];

		address -= this->offset;
		
		if (!(address < 0 || address > 65535))
			modbus_write_bit(modbus_ctx, address, (int)value);

		dStmt.bind(row[0]);
		dResult = dStmt.execute();
	}

	//Zapytanie o rejestry
	qStmt = db->sql(
		"SELECT id, address, value, type FROM writes "
		"WHERE var = 'R'"
	);
	qResult = qStmt.execute();

	//Zapis rejestrów
	while (row = qResult.fetchOne())
	{
		int address = row[1];
		double value = row[2];
		string type = (string) row[3];

		address -= this->offset;
		try {
			PlcVariable item((char*) "R", address, type.c_str(), "");
			item.setRealValue(value);

			modbus_write_registers(modbus_ctx, item.getAddress(), item.getSize(), item.getValue());
		}
		catch (PlcVariable::TypeException& e) {
			std::cout << e.what();
		}
		catch (PlcVariable::AddressException& e) {
			std::cout << e.what();
		}

		dStmt.bind(row[0]);
		dResult = dStmt.execute();
	}
}
