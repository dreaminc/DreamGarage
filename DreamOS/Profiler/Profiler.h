#ifndef PROFILER_H_
#define PROFILER_H_

// DREAM OS
// DreamOS/Dimension/Primitives/Profiler.h
// Profiler

#include "Primitives/valid.h"
#include <chrono>
#include <vector>
#include <deque>

#include "ProfilerGraph.h"

class TickCounter
{
public:
	TickCounter();
	~TickCounter();

	// Submits a tick to the counter
	virtual void Tick();

	// Measure the ticks per second for the last m_nsamples samples
	virtual double	GetTicksPerSecond();

private:
	static const int m_nsamples = 50;
	int	m_currentSample = 0;
	std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>>	m_tickTimes;
};

class Profiler : public valid {
public:
	static Profiler* GetProfiler()
	{
		static Profiler profiler;
		return &profiler;
	}

	Profiler();
	~Profiler();

	void OnFrameRendered();

	typedef ProfilerGraph<uint16_t>	FPSGraph_t;

	FPSGraph_t& GetFPSGraph();

	void AddConsoleLine(const std::string& text);

	const std::deque<std::string>& GetConsoleText();

private:

	TickCounter	m_ticker;
	FPSGraph_t m_FPSGraph{ 3.0 };

	std::deque<std::string>	m_ConsoleText;
	const unsigned int console_max_lines = 200;
};

#endif // !PROFILER_H_
