#ifndef PROFILER_H_
#define PROFILER_H_

// DREAM OS
// DreamOS/Dimension/Primitives/Console.h
// DreamConsole

#include "RESULT/EHM.h"
#include "Sense/SenseKeyboard.h"

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

class DreamConsole : public valid, public Subscriber<SenseKeyboardEvent> {
public:
	static DreamConsole* GetConsole()
	{
		static DreamConsole profiler;
		return &profiler;
	}

	DreamConsole();
	~DreamConsole();

	bool IsInForeground();

	void OnFrameRendered();

	typedef ProfilerGraph<uint16_t>	FPSGraph_t;

	FPSGraph_t& GetFPSGraph();

	void AddConsoleLine(const std::string& text);

	const std::deque<std::string>& GetConsoleText();

	const std::string& GetCmdText();

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;

private:

	// when DreamConsole is in foreground, it is to be displayed as an overlay
	// and capture key input for the console command.
	bool m_isInForeground = false;

	TickCounter	m_ticker;
	FPSGraph_t m_FPSGraph{ 3.0 };

	std::deque<std::string>	m_ConsoleText;
	const unsigned int console_max_lines = 200;

	long long	m_lineCnt = 0;

	std::string	m_cmdText;
};

#endif // !PROFILER_H_
