#ifndef DREAM_NAMED_PIPE_CLIENT_H_
#define DREAM_NAMED_PIPE_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// Sandbox\NamedPipeClient.h
// A named pipe client primitive for IPC
// Currently this is only really something for Windows - so not sure whether 
// or not to make this more general outside of a Dream side object that 
// wraps up the functionality. 

class NamedPipeClient {
public:
	NamedPipeClient(std::wstring wstrPipename);
	~NamedPipeClient();

	virtual RESULT Initialize() = 0;

	RESULT Start();
	RESULT Stop();

	RESULT RegisterMessageHandler(std::function<RESULT(void*, size_t)> fnPipeMessageHandler);
	RESULT UnregisterMessageHandler();

	virtual RESULT Close() = 0;
	virtual RESULT NamedPipeClientProcess() = 0;
	virtual RESULT SendMessage(void *pBuffer, size_t pBuffer_n) = 0;

protected:
	std::wstring m_strPipename;

	std::thread m_namedPipeClientProcess;
	bool m_fRunning = false;

	std::function<RESULT(void*, size_t)> m_fnPipeMessageHandler = nullptr;
};

#endif // ! DREAM_NAMED_PIPE_CLIENT_H_