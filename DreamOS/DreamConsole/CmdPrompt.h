#ifndef CMDPROMPT_H_
#define CMDPROMPT_H_

// DREAM OS
// DreamOS/DreamConsole/CmdPrompt.h
// CmdPrompt


#include "Primitives/valid.h"
#include "RESULT/RESULT.h"

#include "Primitives/Publisher.h"

#include <string>
#include <map>

typedef struct CmdPromptEvent {
	CmdPromptEvent()
	{
	}
} CMD_PROMPT_EVENT;

class CmdPrompt : public valid, public Publisher<std::string, CmdPromptEvent> {
public:
	enum class method {
		DreamApp,
		DreamConsole
	};

private:
	const std::map<method, std::string> methodDictionary {
		{ method::DreamApp, "APP"},
		{ method::DreamConsole, "CONSOLE" },
	};

public:
	static CmdPrompt* GetCmdPrompt()
	{
		static CmdPrompt cmdPrompt;
		cmdPrompt.Init();
		return &cmdPrompt;
	}

	CmdPrompt();
	~CmdPrompt();
	
	RESULT RegisterMethod(CmdPrompt::method method, Subscriber<CmdPromptEvent>* pSubscriber);

	RESULT Execute(const std::string& command);

private:
	void	Init();

private:
	bool	m_isInit = false;
};


#endif // !CMDPROMPT_H_
