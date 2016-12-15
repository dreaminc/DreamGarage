#include "CmdPrompt.h"

#include "DreamConsole/DreamConsole.h"

// CmdPrompt

CmdPrompt::CmdPrompt()
{
	Validate();
	return;
}

CmdPrompt::~CmdPrompt()
{

}

void CmdPrompt::Init()
{
	if (m_isInit)
	{
		return;
	}

	// Initialize singleton

	for (auto& m : methodDictionary)
	{
		RegisterEvent(m.second);
	}

	m_isInit = true;

	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::Cmd, this);
}

RESULT CmdPrompt::RegisterMethod(CmdPrompt::method method, Subscriber<CmdPromptEvent>* pSubscriber)
{
	RESULT r = RegisterSubscriber(methodDictionary.at(method), pSubscriber);

	if (r == R_PASS) {
		m_registeredCommands.push_back(methodDictionary.at(method));
	}

	return r;
}

RESULT CmdPrompt::Execute(const std::string& command)
{
	m_lastExecutedCommand = command;

	std::string type = command.substr(0, command.find(' '));
	
	//HUD_OUT(("Executing " + command + " (" + type + ")").c_str());

	if (type.compare("") != 0)
	{
		CmdPromptEvent event(command);
		NotifySubscribers(type, &event);
	}

	return R_PASS;
}

const std::string& CmdPrompt::GetLastCommand() {
	return m_lastExecutedCommand;
}

RESULT CmdPrompt::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	if (event->GetArg(1).compare("list") == 0) {
		// list of registered commands
		for (auto& cmd : m_registeredCommands) {
			HUD_OUT(cmd.c_str());
		}
	}

	return r;
}