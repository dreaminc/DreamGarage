#ifndef DREAM_NAMED_PIPE_SERVER_H_
#define DREAM_NAMED_PIPE_SERVER_H_

#include "core/ehm/EHM.h"

// Dream Sandbox
// dos/src/sandbox/NamedPipeServer.h

// A named pipe server primitive for IPC
// Currently this is only really something for Windows - so not sure whether 
// or not to make this more general outside of a Dream side object that 
// wraps up the functionality. 

class NamedPipeServer {
public:
	class observer {
	public:
		virtual RESULT OnClientConnect() = 0;
		virtual RESULT OnClientDisconnect() = 0;
	};

	NamedPipeServer(std::wstring wstrPipename);
	~NamedPipeServer();

	virtual RESULT Initialize() = 0;
	virtual RESULT Close() = 0;

	RESULT Start();
	RESULT Stop();

	RESULT RegisterNamedPipeServerObserver(NamedPipeServer::observer* pObserver);
	RESULT RegisterMessageHandler(std::function<RESULT(void*, size_t)> fnPipeMessageHandler);
	RESULT UnregisterMessageHandler();

	virtual RESULT NamedPipeServerProcess() = 0;
	virtual RESULT SendMessage(void *pBuffer, size_t pBuffer_n) = 0;

	virtual RESULT ClearConnections() = 0;

protected:
	std::wstring m_strPipename;

	std::thread m_namedPipeServerProcess;
	bool m_fRunning = false;

	NamedPipeServer::observer* m_pObserver = nullptr;
	std::function<RESULT(void*, size_t)> m_fnPipeMessageHandler = nullptr;
};

#endif // ! DREAM_NAMED_PIPE_SERVER_H_