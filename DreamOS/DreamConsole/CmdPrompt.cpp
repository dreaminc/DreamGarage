#include "CmdPrompt.h"

#include "DreamConsole/DreamConsole.h"

// CmdPrompt

CmdPrompt::CmdPrompt() {
	Validate();
	return;
}

CmdPrompt::~CmdPrompt() {
	// empty
}

RESULT CmdPrompt::Initialize() {
	RESULT r = R_PASS;

	CBM((m_fInit == false), "CMD Prompt already initialized");

	// Initialize singleton
	for (auto& m : methodDictionary) {
		RegisterEvent(m.second);
	}

	m_fInit = true;

Error:
	return r;
}

// TODO: Generalize
RESULT CmdPrompt::RegisterMethod(CmdPrompt::method method, Subscriber<CmdPromptEvent>* pSubscriber) {
	return RegisterSubscriber(methodDictionary.at(method), pSubscriber);
}

RESULT CmdPrompt::Execute(const std::string& command) {
	m_strLastExecutedCommand = command;

	std::string type = command.substr(0, command.find(' '));
	
	//HUD_OUT(("Executing " + command + " (" + type + ")").c_str());

	if (type.compare("") != 0) {
		CmdPromptEvent event(command);
		NotifySubscribers(type, &event);
	}

	return R_PASS;
}

const std::string& CmdPrompt::GetLastCommand() {
	return m_strLastExecutedCommand;
}