#pragma once
#include <atomic>
#include "LogData.h"

class LoggerSettings
{
	private:
		LogData *logData = nullptr;
		int sweep_time = 100;
		bool stop_signal = false;			//< Sygna� zatrzymania w�tku
		std::atomic_bool stopped = false;	//< Czy w�tek zosta� zatrzymany
		bool pause_signal = false;			//< Sygna� wstrzymania w�tku
		std::atomic_bool paused;			//< Czy w�tek zosta� wstrzymany
		bool resume_signal = false;			//< Sygna� wznowienia w�tku
	public:
		LoggerSettings();
		void setLogData(LogData* logData);
		LogData* getLogData();
		void setSweepTime(int sweep_time);
		int getSweepTime();
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

