#ifndef PROFILER_H_
#define PROFILER_H_

// DREAM OS
// DreamOS/Dimension/Primitives/Profiler.h
// Profiler

#include "Primitives/valid.h"
#include <chrono>
#include <vector>

class TickCounter
{
public:
	TickCounter();
	~TickCounter();

	// Submits a tick to the counter
	void Tick();

	// Measure the ticks per second for the last m_nsamples samples
	double	GetTicksPerSecond();

private:
	static const int m_nsamples = 5;
	int	m_currentSample = 0;
	std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>>	m_tickTimes;
};

class Profiler : public TickCounter, public valid {
public:
	Profiler();
	~Profiler();

private:
};

#endif // !PROFILER_H_
