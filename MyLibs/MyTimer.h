#pragma once

#include <ctime>

class MyTimer
{
public:
	MyTimer(void);
	~MyTimer(void);

	void Restart();
	void StartIfNotYet();
	void Stop();
	void Pause();
	void Resume();

	// used time will subtract used time
	// elapse count real world time total
	double GetStopUsedTime() const;
	double GetStopElapsed() const;

	double GetUsedTime() const;
	double GetElapsed() const;

	// static function
	static double GetCurrentTime();

protected:
	
	clock_t mStartClock;
	clock_t mEndClock;

	clock_t mPauseStartClock;
	clock_t mPauseClockTotal;
};

