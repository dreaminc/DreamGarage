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
}

RESULT CmdPrompt::RegisterMethod(CmdPrompt::method method, Subscriber<CmdPromptEvent>* pSubscriber)
{
	return RegisterSubscriber(methodDictionary.at(method), pSubscriber);
}

RESULT CmdPrompt::Execute(const std::string& command)
{
	std::string type = command.substr(0, command.find(' '));
	
	//HUD_OUT(("Executing " + command + " (" + type + ")").c_str());

	if (type.compare("") != 0)
	{
		CmdPromptEvent event(command);
		NotifySubscribers(type, &event);
	}

	return R_PASS;
}
