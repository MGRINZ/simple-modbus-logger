#include "DataWriter.h"

///
/// Konstruktor obiektu zapisuj�cego warto�ci z bazy danych do sterownika
///
/// @param	modbus_ctx	wska�nik do po��czenia MODBUS
/// @param	db			wska�nik do obiektu po��czenia z baz� danych MySQL
/// @param	offset		przesuni�cie adresu w sterowniku
///
DataWriter::DataWriter(modbus_t* modbus_ctx, mysqlx::Session* db, short offset)
{
	this->modbus_ctx = modbus_ctx;
	this->db = db;
	this->offset = offset;
}

///
/// Zapis nowych warto�ci do sterownika
/// Metoda pobiera rekordy z bazy danych MySQL, przetwarza je i zapisuje w sterowniku
/// za pomoc� protoko�u MODBUS.
///
void DataWriter::write()
{
	mysqlx::SqlResult qResult;	//< Wynik zapytania
	mysqlx::SqlResult dResult;	//< Wynik usuwania
	mysqlx::Row row;			//< Pojedynczy rekord

	//Polecenie usuni�cia
	mysqlx::SqlStatement dStmt = db->sql(
		"DELETE FROM writes"
	);
	
	//Zapytanie o wyj�cia
	mysqlx::SqlStatement qStmt = db->sql(
		"SELECT a.id, address, value, type FROM writes "
		"JOIN ( "
		"    SELECT MAX(id) AS id FROM writes "
		"    WHERE var = 'Q' "
		"    GROUP BY address "
		") AS a "
		"ON writes.id = a.id "
	);
	qResult = qStmt.execute();

	//Zapis wyj�� dyskretnych
	while (row = qResult.fetchOne()) {
		int address = row[1];
		double value = row[2];

		address -= this->offset;
		
		if (!(address < 0 || address > 65535))
			modbus_write_bit(modbus_ctx, address, (int)value);		
	}

	//Zapytanie o rejestry
	qStmt = db->sql(
		"SELECT a.id, address, value, type FROM writes "
		"JOIN ( "
		"    SELECT MAX(id) AS id FROM writes "
		"    WHERE var = 'R' "
		"    GROUP BY address "
		") AS a "
		"ON writes.id = a.id "
	);
	qResult = qStmt.execute();

	//Zapis rejestr�w
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
	}

	dResult = dStmt.execute();
}
