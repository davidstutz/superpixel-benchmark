//////////////////////////////////////////////////////////////////////////////
// Timer.h
// =======
// High Resolution Timer.
// This timer is able to measure the elapsed time with 1 micro-second accuracy
// in both Windows, Linux and Unix system
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2003-01-13
// UPDATED: 2006-01-13
//
// Copyright (c) 2003 Song Ho Ahn
//
// Modified by David Weikersdorfer (davidw@danvil.de), 16. Jun 2009
//
//////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
#ifndef DANVIL_TOOLS_TIMER_H_DEF
#define DANVIL_TOOLS_TIMER_H_DEF
//---------------------------------------------------------------------------
#ifdef WIN32
	// Windows system specific
	#include <windows.h>
#else
	// Unix based system specific
	#include <sys/time.h>
#endif
#include <stdlib.h>
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------

/** High resolution timer for linux and windows
 */
class Timer
{
public:
#ifdef WIN32
	typedef LARGE_INTEGER TimeType;
#else
	typedef timeval TimeType;
#endif

public:
	/** constructor initializes */
	Timer()
	{
#ifdef WIN32
	    QueryPerformanceFrequency(&frequency);
	    startCount.QuadPart = 0;
	    endCount.QuadPart = 0;
#else
	    startCount.tv_sec = startCount.tv_usec = 0;
	    endCount.tv_sec = endCount.tv_usec = 0;
#endif
	    stopped = 0;
	    startTimeInMicroSec = 0;
	    endTimeInMicroSec = 0;
	}

	/** no clean up necessary */
	~Timer() {
	}

	/** Current time in microseconds */
	static long long int GetCurrentTime() {
#ifdef WIN32
		LARGE_INTEGER a;
		LARGE_INTEGER frequency;
		QueryPerformanceCounter(&a);
		QueryPerformanceFrequency(&frequency);
		return (a.QuadPart * 1000000) / frequency.QuadPart;
#else
		timeval a;
		gettimeofday(&a, NULL);
		return a.tv_sec * 1000000 + a.tv_usec;
#endif
	}

	/** Current time in seconds */
	static double GetCurrentTimeSeconds() {
		return (double)(GetCurrentTime()) / 1000000.0f;
	}

	/** start timer.
	 * startCount will be set at this point.
	 */
	void start()
	{
	    stopped = 0; // reset stop flag
	#ifdef WIN32
	    QueryPerformanceCounter(&startCount);
	#else
	    gettimeofday(&startCount, NULL);
	#endif
	}

	/** stop the timer.
	 * endCount will be set at this point.
	 */
	void stop()
	{
	    stopped = 1; // set timer stopped flag

	#ifdef WIN32
	    QueryPerformanceCounter(&endCount);
	#else
	    gettimeofday(&endCount, NULL);
	#endif
	}

	/** compute elapsed time in micro-second resolution.
	 * other getElapsedTime will call this first, then convert to correspond resolution.
	 */
	double getElapsedTimeInMicroSec()
	{
#ifdef WIN32
	    if(!stopped)
	        QueryPerformanceCounter(&endCount);

	    startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
	    endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
	    if(!stopped)
	        gettimeofday(&endCount, NULL);

	    startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
	    endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif

	    return endTimeInMicroSec - startTimeInMicroSec;
	}

	/** get elapsed time in milliseconds
	 * divide elapsedTimeInMicroSec by 1000
	 */
	double getElapsedTimeInMilliSec()
	{
	    return this->getElapsedTimeInMicroSec() * 0.001;
	}

	/** get elapsed time in seconds
	 * divide elapsedTimeInMicroSec by 1000000
	 */
	double getElapsedTimeInSec()
	{
	    return this->getElapsedTimeInMicroSec() * 0.000001;
	}

	/** same as getElapsedTimeInSec() */
	double getElapsedTime()
	{
	    return this->getElapsedTimeInSec();
	}

private:
	/** starting time in micro-second */
	double startTimeInMicroSec;

	/** ending time in micro-second */
	double endTimeInMicroSec;

	/** stop flag */
	int stopped;

#ifdef WIN32
	/** ticks per second */
	LARGE_INTEGER frequency;

	/** start time */
	LARGE_INTEGER startCount;

	/** end time */
	LARGE_INTEGER endCount;
#else
	/** start time */
	timeval startCount;

	/** end time */
	timeval endCount;
#endif
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif // DANVIL_TOOLS_TIMER_H_DEF
