#pragma once
#include <atomic>
#include "LogData.h"
#include "DataWriter.h"

class LoggerSettings
{
	private:
		LogData *logData = nullptr;
		DataWriter *dataWriter = nullptr;
		short sweep_time = 100;
		bool stop_signal = false;			//< Sygna� zatrzymania w�tku
		std::atomic_bool stopped = false;	//< Czy w�tek zosta� zatrzymany
		bool pause_signal = false;			//< Sygna� wstrzymania w�tku
		std::atomic_bool paused;			//< Czy w�tek zosta� wstrzymany
		bool resume_signal = false;			//< Sygna� wznowienia w�tku
	public:
		LoggerSettings();
		void setLogData(LogData* logData);
		void setDataWriter(DataWriter* dataWriter);
		LogData* getLogData();
		DataWriter* getDataWriter();
		void setSweepTime(short sweep_time);
		short getSweepTime();
		void stop();
		bool isStopSignalSent();
		void setStopped(bool value);
		bool isStopped();
		void pause();
		bool isPauseSignalSent();
		void setPaused(bool value);
		bool isPaused();
		void resume();
		bool isResumeSignalSent();
};

