#include "easylogging++.h"
#include "Console.h"
#include "ProfilerGraph.h"

#include <algorithm>

#include "DreamConsole/DreamConsole.h"

// DreamConsole

DreamConsole::DreamConsole() {
	Validate();
	return;
}

DreamConsole::~DreamConsole() {
	// empty
}

const DreamConsole::Configuration& DreamConsole::GetConfiguration()
{
	return m_configuration;
}

RESULT DreamConsole::Initialize() {
	RESULT r = R_PASS;

	CB((m_fInitialized == false));

	// Initialize singleton
	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::DreamConsole, this);

	m_fInitialized = true;

Error:
	return r;
}

bool DreamConsole::IsInForeground() {
	return m_fInForeground;
}

void DreamConsole::OnFrameRendered() {
	m_ticker.Tick();

	m_FPSGraph.AddMeasurement(static_cast<uint16_t>(m_ticker.GetTicksPerSecond()));

	return;
}

DreamConsole::FPSGraph_t& DreamConsole::GetFPSGraph() {
	return m_FPSGraph;
}

void DreamConsole::AddConsoleLine(const std::string& text) {
	std::lock_guard<std::mutex> lock(m_mutex);

	m_ConsoleText.push_back(std::to_string(m_lineCnt++) + " " + text);

	while (m_ConsoleText.size() > console_max_lines) {
		m_ConsoleText.pop_front();
	}
}

void DreamConsole::ForEach(std::function<bool(const std::string)> pred) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::for_each(m_ConsoleText.rbegin(), m_ConsoleText.rend(), pred);
}

const std::deque<std::string>& DreamConsole::GetConsoleText() {
	return m_ConsoleText;
}

const std::string& DreamConsole::GetCmdText() {
	return m_cmdText;
}

unsigned int DreamConsole::GetCmtTextCursorPos() {
	return m_cmdTextCursorPos;
}

void DreamConsole::TextCursorMoveFront() {
	m_cmdTextCursorPos = static_cast<unsigned int>(m_cmdText.length());
}

void DreamConsole::TextCursorMoveBack() {
	m_cmdTextCursorPos = 0;
}

void DreamConsole::TextCursorMoveBackward() {
	if (m_cmdTextCursorPos > 0)
		m_cmdTextCursorPos--;
}

void DreamConsole::TextCursorMoveForward() {
	if (m_cmdTextCursorPos < m_cmdText.length())
		m_cmdTextCursorPos++;
}

RESULT DreamConsole::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	if (kbEvent->m_pSenseKeyboard) {
		SenseVirtualKey keyCode = kbEvent->KeyCode;

		if (kbEvent->KeyState) {
			if (!IsInForeground()) {
				if (keyCode == SVK_TAB) {
					// quick hack to enable dream console in production but only using several tab hits
#ifdef PRODUCTION_BUILD
					static int hits = 0;
					hits++;
					if (hits > 7)
						m_fInForeground = true;
#else
					m_fInForeground = true;
#endif // PRODUCTION_BUILD
				}
			}
			else {
				if (keyCode == SVK_TAB) {
					m_fInForeground = false;
				}
				else {
					///*** old way
					switch (keyCode) {

						case SVK_LEFT: TextCursorMoveBackward(); break;
						case SVK_RIGHT: TextCursorMoveForward(); break;
					
						// Don't process type character here. look for SenseTypingEvent
						case SVK_BACK: 
						case SVK_RETURN: 
						case SVK_UP:
						case SVK_DOWN: 
						case SVK_ESCAPE: 
						default: break;
					}
					//*/
				}
			}
		}
	}

	//Error:
	return r;
}

RESULT DreamConsole::Notify(SenseTypingEvent *kbEvent) {
	RESULT r = R_PASS;

	if (IsInForeground()) {
		switch (kbEvent->u16character) {
			case SVK_BACK:
				// Process a backspace. 
				if (!m_cmdText.empty() && GetCmtTextCursorPos() > 0) {
					m_cmdText.erase(GetCmtTextCursorPos() - 1, 1);
					TextCursorMoveBackward();
				}

				break;

			case 0x0A: {
				// Process a linefeed. 
			} break;

			case SVK_ESCAPE:
				// Process an escape. 
				if (!m_cmdText.empty()) {
					m_cmdText.erase();
					TextCursorMoveBack();
				}
				else {
					m_cmdText = CmdPrompt::GetCmdPrompt()->GetLastCommand();
					TextCursorMoveFront();
				}
				break;

			case SVK_TAB: {
				// Process a tab. 
			} break;

			case SVK_RETURN: {
				// Process a carriage return. 
				HUD_OUT((std::string("cmd: ") + m_cmdText).c_str());
				CMDPROMPT_EXECUTE(m_cmdText);
				m_cmdText.erase();
				TextCursorMoveBack();
			} break;

			default: {
				// Process displayable characters. 
				std::string nonUnicodeChar = utf16_to_utf8(std::u16string(1, kbEvent->u16character));
				m_cmdText.insert(GetCmtTextCursorPos(), 1, nonUnicodeChar[0]);
				TextCursorMoveForward();
			} break;
		}
	}

	return r;
}

RESULT DreamConsole::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	if (event->GetArg(1).compare("list") == 0) {
		HUD_OUT("graph fps/off : show / hide fps graph");
	}

	if (event->GetArg(1).compare("graph") == 0) {
		if (event->GetArg(2).compare("fps") == 0) {
			m_configuration.graph = GraphConfiguration::FPS;
			HUD_OUT("consol graph <- fps");
		}
		else if (event->GetArg(2).compare("off") == 0) {
			m_configuration.graph = GraphConfiguration::FPSMinimal;
			HUD_OUT("consol graph <- off");
		}
	}

	return r;
}

// TickCounter
TickCounter::TickCounter() {
	m_tickTimes.resize(m_nsamples);
}

TickCounter::~TickCounter() {
	// empty
}

void TickCounter::Tick() {
	m_tickTimes[m_currentSample++] = std::chrono::high_resolution_clock::now();

	if (m_currentSample >= m_nsamples)
		m_currentSample = 0;
}

double	TickCounter::GetTicksPerSecond() {
	auto result = std::minmax_element(m_tickTimes.begin(), m_tickTimes.end());

	double elapsedTime = std::chrono::duration<double>(m_tickTimes[result.second - m_tickTimes.begin()] 
		- m_tickTimes[result.first - m_tickTimes.begin()]).count();

	return (m_nsamples - 1) / elapsedTime;
}


