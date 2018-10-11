#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Pause();
	void Play();

	Uint32 Read(); //const?
	float ReadSec();

private:

	bool	running;
	Uint32	started_at;
	Uint32	paused_at;
};

class PerfTimer
{
public:

	PerfTimer();

	void Start();

	double ReadMs() const;

	void Pause();

	void Play();

private:

	double paused_at = 0;
	double started_at = 0;
	bool running;
	static Uint64 frequency;
};


#endif //__TIMER_H__