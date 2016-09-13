#include "DreamConsole/Console.h"
#include "DreamConsole/CmdPrompt.h"

#include <string>

#define	HUDOUT_MAX_SIZE	1024

#define HUD_OUT(str, ...) do { \
	static char outstr[HUDOUT_MAX_SIZE];\
	sprintf_s(outstr, HUDOUT_MAX_SIZE, str, ##__VA_ARGS__); if (outstr[0] != '\n' && outstr[0] != '\r') DreamConsole::GetConsole()->AddConsoleLine(std::string(outstr)); } while(0);

#define CMDPROMPT_EXECUTE(str) CmdPrompt::GetCmdPrompt()->Execute(str);

// Overlay Debug Console
#include "DreamConsole/DebugConsole.h"
/* why is this not working? _Pragma("warning(suppress: 4533)") \*/
#pragma warning(disable:4533)

#define OVERLAY_DEBUG_OUT(str) \
	static std::shared_ptr<DebugData> pDebugData = DebugConsole::GetDebugConsole()->Register(); \
	pDebugData->SetValue(str)

#define OVERLAY_DEBUG_SET(name,str) \
	DebugConsole::GetDebugConsole()->Get(name)->SetValue(str)

#define SHORT_STR(str) \
   ((str.length() > 20) ? str.substr(0, 17) + "..." : str)