#ifndef CMDPROMPT_H_
#define CMDPROMPT_H_

// DREAM OS
// DreamOS/DreamConsole/CmdPrompt.h
// CmdPrompt


#include "Primitives/valid.h"
#include "RESULT/RESULT.h"

#include "Primitives/Publisher.h"

#include <string>
#include <sstream>
#include <map>
#include <vector>

typedef struct CmdPromptEvent {
	CmdPromptEvent(const std::string& command) {
		std::string arg;
		std::stringstream ss(command);

		while (ss >> arg) {
			m_args.push_back(arg);
		}
	}

	const std::string& GetArg(size_t index) {
		return (index > 0 && index < m_args.size()) ? m_args[index] : k_empty;
	}

	const std::string k_empty{ "" };
	std::vector<std::string>	m_args;
} CMD_PROMPT_EVENT;

class CmdPrompt : public valid, public Publisher<std::string, CmdPromptEvent>, public Subscriber<CmdPromptEvent> {
public:
	enum class method {
		Cmd,
		DreamApp,
		DreamConsole,
		CloudController,
		OpenGL,
		Camera,
		Leap
	};

private:
	const std::map<method, std::string> methodDictionary {
		{ method::Cmd, "cmd" },
		{ method::DreamApp, "app"},
		{ method::DreamConsole, "console" },
		{ method::CloudController, "cloud" },
		{ method::OpenGL, "ogl" },
		{ method::Camera, "cam" }, 
		{ method::Leap, "leap"}
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

	const std::string& GetLastCommand();

	RESULT Notify(CmdPromptEvent *event);

private:
	void	Init();

private:
	bool	m_isInit = false;

	std::string	m_lastExecutedCommand;

	std::vector<std::string> m_registeredCommands;
};


#endif // !CMDPROMPT_H_
