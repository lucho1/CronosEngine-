// ----------------------------------------------------
// PerfTimer.cpp
// Slow timer with microsecond precision
// ----------------------------------------------------
#include "Providers/cnpch.h"

#include "PerfTimer.h"

#include "SDL\include\SDL_timer.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	uint64 PerfTimer::frequency = 0;

	// ---------------------------------------------
	PerfTimer::PerfTimer()
	{
		if (frequency == 0)
			frequency = SDL_GetPerformanceFrequency();

		Start();
	}

	// ---------------------------------------------
	void PerfTimer::Start()
	{
		started_at = SDL_GetPerformanceCounter();
	}

	// ---------------------------------------------
	double PerfTimer::ReadMs() const
	{
		return 1000.0 * (double(SDL_GetPerformanceCounter() - started_at) / double(frequency));
	}

	// ---------------------------------------------
	uint64 PerfTimer::ReadTicks() const
	{
		return (SDL_GetPerformanceCounter() - started_at);
	}

}