#include "Profiler.h"

#include <algorithm>

// Profiler

Profiler::Profiler()
{
	Validate();
	return;
}

Profiler::~Profiler()
{

}

// TickCounter
TickCounter::TickCounter()
{
	m_tickTimes.resize(m_nsamples);
}

TickCounter::~TickCounter()
{

}

void TickCounter::Tick()
{
	m_tickTimes[m_currentSample++] = std::chrono::high_resolution_clock::now();

	if (m_currentSample >= m_nsamples)
	{
		m_currentSample = 0;
	}
}

double	TickCounter::GetTicksPerSecond()
{
	auto result = std::minmax_element(m_tickTimes.begin(), m_tickTimes.end());

	double elapsedTime = std::chrono::duration<double>(m_tickTimes[result.second - m_tickTimes.begin()] 
		- m_tickTimes[result.first - m_tickTimes.begin()]).count();

	return (m_nsamples - 1) / elapsedTime;
}

