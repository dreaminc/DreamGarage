#ifndef PROFILER_H_
#define PROFILER_H_

// DREAM OS
// DreamOS/Dimension/Primitives/Console.h
// DreamConsole

#include "RESULT/EHM.h"
#include "Sense/SenseKeyboard.h"
#include "DreamConsole/CmdPrompt.h"

#include "Primitives/valid.h"
#include <chrono>
#include <vector>
#include <deque>
#include <mutex>

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

class DreamConsole : public valid, public Subscriber<SenseKeyboardEvent>, public Subscriber<SenseTypingEvent>, public Subscriber<CmdPromptEvent> {
public:
	static DreamConsole* GetConsole()
	{
		static DreamConsole console;
		console.Init();
		return &console;
	}

	DreamConsole();
	~DreamConsole();

public:
	enum class GraphConfiguration {
		FPS,
		FPSMinimal
	};

	struct Configuration {
		GraphConfiguration	graph{ GraphConfiguration::FPSMinimal };
	};

	const Configuration& GetConfiguration();

public:
	void Init();

	bool IsInForeground();

	void OnFrameRendered();

	typedef ProfilerGraph<uint16_t>	FPSGraph_t;

	FPSGraph_t& GetFPSGraph();

	void AddConsoleLine(const std::string& text);

	const std::deque<std::string>& GetConsoleText();

	void ForEach(std::function<bool(const std::string)> pred);

	const std::string& GetCmdText();
	unsigned int GetCmtTextCursorPos();

	// SenseKeyboardEventSubscriber
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(SenseTypingEvent *kbEvent) override;

	// CmdPromptEventSubscriber
	virtual RESULT Notify(CmdPromptEvent *event) override;

private:
	void TextCursorMoveFront();
	void TextCursorMoveBack();
	void TextCursorMoveBackward();
	void TextCursorMoveForward();

private:
	bool m_isInit = false;

	// when DreamConsole is in foreground, it is to be displayed as an overlay
	// and capture key input for the console command.
	bool m_isInForeground = false;

	TickCounter	m_ticker;
	FPSGraph_t m_FPSGraph{ 3.0 };

	std::deque<std::string>	m_ConsoleText;
	const unsigned int console_max_lines = 200;

	long long	m_lineCnt = 0;

	std::mutex m_mutex;
	std::string	m_cmdText;
	unsigned int m_cmdTextCursorPos = 0;

	Configuration m_configuration;
};

#endif // !PROFILER_H_
