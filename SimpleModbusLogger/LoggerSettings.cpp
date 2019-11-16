#include "LoggerSettings.h"

LoggerSettings::LoggerSettings()
{
}

///
/// Ustawienie listy zmiennych do odczytu
///
/// @param	logData	obiekt listy zmiennych
///
void LoggerSettings::setLogData(LogData* logData)
{
	this->logData = logData;
}

///
/// Ustawienie obiektu zapisuj�cego dane do sterownika
///
/// @param	dataWriter	obiekt zapisuj�cy dane
///
void LoggerSettings::setDataWriter(DataWriter* dataWriter)
{
	this->dataWriter = dataWriter;
}

///
/// Pobranie aktualnej listy zmiennych do odczytu
///
/// @return		obiekt listy zmiennych do odczytu
///
LogData* LoggerSettings::getLogData()
{
	return logData;
}

///
/// Pobranie obiektu zapisuj�cego dane do sterownika
///
/// @return		obiekt zapisuj�cy dane
///
DataWriter* LoggerSettings::getDataWriter()
{
	return dataWriter;
}

///
/// Ustawienie czasu trwania cyklu
///
/// @param	sweep_time	czas trwania cyklu
///
void LoggerSettings::setSweepTime(short sweep_time)
{
	this->sweep_time = sweep_time;
}

///
/// Pobranie aktualnego czasu trwania cyklu
///
/// @return		czas trwania cyklu
///
short LoggerSettings::getSweepTime()
{
	return sweep_time;
}

///
/// Ustawienie wy�wietlania czasu trwania cyklu w oknie programu
///
/// @param	show_time	stan wy�wietlania czasu trwania cyklu
///
void LoggerSettings::showTime(bool show_time)
{
	this->show_time = show_time;
}

///
/// Pobieranie stanu flagi wy�wietlania czasu trwania cyklu w oknie programu
///
/// @return		stan wy�wietlania czasu trwania cyklu
///
bool LoggerSettings::showTime()
{
	return this->show_time;
}

///
/// Wys�anie sygna�u zatrzymania w�tku
///
void LoggerSettings::stop()
{
	stop_signal = true;
}

///
/// Sprawdza czy zosta� wys�any sygna� zatrzymania w�tku
///
/// @return		warto�� sygna�u zatrzymania w�tku
///
bool LoggerSettings::isStopSignalSent()
{
	return stop_signal;
}

///
/// Ustawia czy w�tek zosta� zatrzymany
///
void LoggerSettings::setStopped(bool value)
{
	this->stopped = value;
}

///
/// Sprawdzenie czy w�tek zosta� zatrzymany
/// po wys�aniu sygna�u zatrzymania
///
/// @return		stan zatrzymania w�tku
///
bool LoggerSettings::isStopped()
{
	return stopped;
}

///
/// Wys�anie sygna�u wstrzymania w�tku
///
void LoggerSettings::pause()
{
	this->pause_signal = true;
	this->resume_signal = false;
	while (!paused);
}

///
/// Sprawdza czy zosta� wys�any sygna� wstrzymania w�tku
///
/// @return		warto�� sygna�u wstrzymania w�tku
///
bool LoggerSettings::isPauseSignalSent()
{
	return this->pause_signal;
}

///
/// Ustawia czy w�tek zosta� wstrzymany
///
/// @param	value	stan wstrzymania w�tku
///
void LoggerSettings::setPaused(bool value)
{
	this->paused = value;
}

///
/// Sprawdzenie czy w�tek zosta� wstrzymany po wys�aniu sygna�u wstrzymania
/// lub wznowiony po wys�aniu sygna�u wznowienia
///
/// @return		stan wstrzymania w�tku
///
bool LoggerSettings::isPaused()
{
	return this->paused;
}

///
/// Wys�anie sygna�u wznowienia w�tku
///
void LoggerSettings::resume()
{
	this->pause_signal = false;
	this->resume_signal = true;
	while (paused);
}

///
/// Sprawdza czy zosta� wys�any sygna� wznowienia w�tku
///
/// @return		warto�� sygna�u wznowienia w�tku
///
bool LoggerSettings::isResumeSignalSent()
{
	return this->resume_signal;
}
