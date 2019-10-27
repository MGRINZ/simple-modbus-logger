#include "DataWriter.h"

DataWriter::DataWriter(modbus_t* modbus_ctx, mysqlx::Session* db)
{
	this->modbus_ctx = modbus_ctx;
	this->db = db;
}

void DataWriter::write()
{
	mysqlx::SqlResult qResult;	//< Wynik zapytania
	mysqlx::SqlResult dResult;	//< Wynik usuwania
	mysqlx::Row row;			//< Pojedynczy rekord

	//Polecenie usuni�cia
	mysqlx::SqlStatement dStmt = db->sql(
		"DELETE FROM writes "
		"WHERE id = ?"
	);
	
	//Zapytanie o wyj�cia
	mysqlx::SqlStatement qStmt = db->sql(
		"SELECT id, address, value FROM writes "
		"WHERE var = 'Q'"
	);
	qResult = qStmt.execute();

	//Zapis wyj�� dyskretnych
	while (row = qResult.fetchOne()) {
		double value = row[2];

		modbus_write_bit(modbus_ctx, row[1], (int)value);
		dStmt.bind(row[0]);
		dResult = dStmt.execute();
	}

	//Zapytanie o rejestry
	qStmt = db->sql(
		"SELECT id, address, value, type FROM writes "
		"WHERE var = 'R'"
	);
	qResult = qStmt.execute();

	//Zapis rejestr�w
	while (row = qResult.fetchOne())
	{
		double value = row[2];
		string type = (string) row[3];
		PlcVariable item((char*) "R", (int) row[1], type.c_str(), "");
		item.setRealValue(value);

		modbus_write_registers(modbus_ctx, item.getAddress(), item.getSize(), item.getValue());
		dStmt.bind(row[0]);
		dResult = dStmt.execute();
	}
}
