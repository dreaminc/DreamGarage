#include "Console.h"
#include "ProfilerGraph.h"

#include <algorithm>
#include <locale>

#include "DreamConsole/DreamConsole.h"

// DreamConsole

DreamConsole::DreamConsole()
{
	Validate();
	return;
}

DreamConsole::~DreamConsole()
{

}

void DreamConsole::Init()
{
	if (m_isInit)
	{
		return;
	}

	// Initialize singleton
	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::DreamConsole, this);

	m_isInit = true;
}

bool DreamConsole::IsInForeground()
{
	return m_isInForeground;
}

void DreamConsole::OnFrameRendered()
{
	m_ticker.Tick();

	m_FPSGraph.AddMeasurement(static_cast<uint16_t>(m_ticker.GetTicksPerSecond()));

	return;
}

DreamConsole::FPSGraph_t& DreamConsole::GetFPSGraph()
{
	return m_FPSGraph;
}

void DreamConsole::AddConsoleLine(const std::string& text)
{
	m_ConsoleText.push_back(std::to_string(m_lineCnt++) + " " + text);
	while (m_ConsoleText.size() > console_max_lines)
	{
		m_ConsoleText.pop_front();
	}
}

const std::deque<std::string>& DreamConsole::GetConsoleText()
{
	return m_ConsoleText;
}

const std::string& DreamConsole::GetCmdText()
{
	return m_cmdText;
}

RESULT DreamConsole::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	if (kbEvent->m_pSenseKeyboard)
	{
		SK_SCAN_CODE keyCode = kbEvent->KeyCode;

		if (kbEvent->KeyState)
		{
			if (!IsInForeground())
			{
				if (keyCode == VK_TAB)
				{
					m_isInForeground = true;
				}
			}
			else
			{
				if (keyCode == VK_TAB)
				{
					m_isInForeground = false;
				}
				else
				{
					switch (keyCode)
					{
					case VK_BACK: {
						if (!m_cmdText.empty())
							m_cmdText.pop_back();
					} break;
					case VK_RETURN: {
						HUD_OUT((std::string("cmd: ") + m_cmdText).c_str());
						CMDPROMPT_EXECUTE(m_cmdText);
						m_cmdText.erase();
					} break;
					case VK_ESCAPE: {
						m_cmdText.erase();
					} break;
					case VK_LEFT:
					case VK_RIGHT:
					case VK_UP:
					case VK_DOWN: {

					} break;
					default: {
						std::locale	loc;
						m_cmdText.append(std::string("") + std::tolower(static_cast<char>(keyCode), loc));
					} break;
					}
				}
			}
		}
	}

	//Error:
	return r;
}

RESULT DreamConsole::Notify(CmdPromptEvent *kbEvent) {
	RESULT r = R_PASS;

	HUD_OUT("DreamConsole command");

	return r;
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


