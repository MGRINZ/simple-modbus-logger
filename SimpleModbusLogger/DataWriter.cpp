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

	//Polecenie usuniêcia
	mysqlx::SqlStatement dStmt = db->sql(
		"DELETE FROM writes "
		"WHERE id = ?"
	);
	
	//Zapytanie o wyjœcia
	mysqlx::SqlStatement qStmt = db->sql(
		"SELECT id, address, value FROM writes "
		"WHERE type = 'Q'"
	);
	qResult = qStmt.execute();

	while (row = qResult.fetchOne()) {
		modbus_write_bit(modbus_ctx, row[1], row[2]);
		dStmt.bind(row[0]);
		dResult = dStmt.execute();
	}

	//Zapytanie o rejestry
	qStmt = db->sql(
		"SELECT id, address, value FROM writes "
		"WHERE type = 'R'"
	);
	qResult = qStmt.execute();

	while (row = qResult.fetchOne())
	{
		modbus_write_register(modbus_ctx, row[1], (uint16_t) ((int)row[2]));
		dStmt.bind(row[0]);
		dResult = dStmt.execute();
	}
}
