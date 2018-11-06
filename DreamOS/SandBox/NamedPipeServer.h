#ifndef DREAM_NAMED_PIPE_SERVER_H_
#define DREAM_NAMED_PIPE_SERVER_H_

#include "RESULT/EHM.h"

// DREAM OS
// Sandbox\NamedPipeServer.h
// A named pipe server primitive for IPC
// Currently this is only really something for Windows - so not sure whether 
// or not to make this more general outside of a Dream side object that 
// wraps up the functionality. 

class NamedPipeServer {
public:
	NamedPipeServer(std::wstring wstrPipename);
	~NamedPipeServer();

	virtual RESULT Initialize() = 0;
	
	RESULT Start();
	RESULT Stop();

	virtual RESULT NamedPipeServerProcess() = 0;

protected:
	std::wstring m_strPipename;

	std::thread m_namedPipeServerProcess;
	bool m_fRunning = false;
};

#endif // ! DREAM_NAMED_PIPE_SERVER_H_