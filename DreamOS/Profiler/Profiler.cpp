#include "Profiler.h"
#include "ProfilerGraph.h"

#include <algorithm>

#include "windows.h"

// Profiler

Profiler::Profiler()
{
	Validate();
	return;
}

Profiler::~Profiler()
{

}

void Profiler::OnFrameRendered()
{
	m_ticker.Tick();

	m_FPSGraph.AddMeasurement(static_cast<uint16_t>(m_ticker.GetTicksPerSecond()));

	return;
}

Profiler::FPSGraph_t& Profiler::GetFPSGraph()
{
	return m_FPSGraph;
}

void Profiler::AddConsoleLine(const std::string& text)
{
	m_ConsoleText.push_back(text);
	while (m_ConsoleText.size() > console_max_lines)
	{
		m_ConsoleText.pop_front();
	}
}

const std::deque<std::string>& Profiler::GetConsoleText()
{
	return m_ConsoleText;
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


