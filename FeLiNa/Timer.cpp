// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
}

// ---------------------------------------------
void Timer::Pause()
{
	running = false;
	paused_at = SDL_GetTicks();
}

void Timer::Play() {

	running = true;
	started_at += SDL_GetTicks() - paused_at;

}

// ---------------------------------------------
Uint32 Timer::Read()
{
	if(running == true)
	{
		return SDL_GetTicks() - started_at;
	}
	else
	{
		return paused_at - started_at;
	}
}

float Timer::ReadSec()
{
	if (running == true)
	{
		return float (SDL_GetTicks() - started_at)/1000;
	}
	else
	{
		return float (paused_at - started_at)/1000;
	}
}


Uint64 PerfTimer::frequency = 0;

PerfTimer::PerfTimer()
{
	if (frequency == 0)
		frequency = SDL_GetPerformanceFrequency();

	Start();
}

void PerfTimer::Start()
{
	started_at = SDL_GetPerformanceCounter();
}

double PerfTimer::ReadMs() const
{
	if (running) {
		return 1000.0 * (double(SDL_GetPerformanceCounter() - started_at) / double(frequency));
	}

	else {
		return paused_at - started_at;
	}
}


void PerfTimer::Pause() {

	running = false;
	paused_at = SDL_GetPerformanceCounter();

}

void PerfTimer::Play() {
	
	running = true;
	started_at += SDL_GetPerformanceCounter() - paused_at;

}


