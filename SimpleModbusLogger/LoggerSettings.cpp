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
/// Ustawienie obiektu zapisuj¹cego dane do sterownika
///
/// @param	dataWriter	obiekt zapisuj¹cy dane
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
/// Pobranie obiektu zapisuj¹cego dane do sterownika
///
/// @return		obiekt zapisuj¹cy dane
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
/// Ustawienie wyœwietlania czasu trwania cyklu w oknie programu
///
/// @param	show_time	stan wyœwietlania czasu trwania cyklu
///
void LoggerSettings::showTime(bool show_time)
{
	this->show_time = show_time;
}

///
/// Pobieranie stanu flagi wyœwietlania czasu trwania cyklu w oknie programu
///
/// @return		stan wyœwietlania czasu trwania cyklu
///
bool LoggerSettings::showTime()
{
	return this->show_time;
}

///
/// Wys³anie sygna³u zatrzymania w¹tku
///
void LoggerSettings::stop()
{
	stop_signal = true;
}

///
/// Sprawdza czy zosta³ wys³any sygna³ zatrzymania w¹tku
///
/// @return		wartoœæ sygna³u zatrzymania w¹tku
///
bool LoggerSettings::isStopSignalSent()
{
	return stop_signal;
}

///
/// Ustawia czy w¹tek zosta³ zatrzymany
///
void LoggerSettings::setStopped(bool value)
{
	this->stopped = value;
}

///
/// Sprawdzenie czy w¹tek zosta³ zatrzymany
/// po wys³aniu sygna³u zatrzymania
///
/// @return		stan zatrzymania w¹tku
///
bool LoggerSettings::isStopped()
{
	return stopped;
}

///
/// Wys³anie sygna³u wstrzymania w¹tku
///
void LoggerSettings::pause()
{
	this->pause_signal = true;
	this->resume_signal = false;
	while (!paused);
}

///
/// Sprawdza czy zosta³ wys³any sygna³ wstrzymania w¹tku
///
/// @return		wartoœæ sygna³u wstrzymania w¹tku
///
bool LoggerSettings::isPauseSignalSent()
{
	return this->pause_signal;
}

///
/// Ustawia czy w¹tek zosta³ wstrzymany
///
/// @param	value	stan wstrzymania w¹tku
///
void LoggerSettings::setPaused(bool value)
{
	this->paused = value;
}

///
/// Sprawdzenie czy w¹tek zosta³ wstrzymany po wys³aniu sygna³u wstrzymania
/// lub wznowiony po wys³aniu sygna³u wznowienia
///
/// @return		stan wstrzymania w¹tku
///
bool LoggerSettings::isPaused()
{
	return this->paused;
}

///
/// Wys³anie sygna³u wznowienia w¹tku
///
void LoggerSettings::resume()
{
	this->pause_signal = false;
	this->resume_signal = true;
	while (paused);
}

///
/// Sprawdza czy zosta³ wys³any sygna³ wznowienia w¹tku
///
/// @return		wartoœæ sygna³u wznowienia w¹tku
///
bool LoggerSettings::isResumeSignalSent()
{
	return this->resume_signal;
}
