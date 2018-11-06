#ifndef WIN_64_NAMED_PIPE_H_
#define WIN_64_NAMED_PIPE_H_


#include "RESULT/EHM.h"

// DREAM OS
// Sandbox/Windows/Win64NamedPipe.h
// Named pipe capability in windows 

#include "Sandbox/NamedPipeServer.h"

#include <windows.h> 

#define DEFAULT_NAMED_PIPE_BUFFER_SIZE 1024

class Win64NamedPipeServer : public NamedPipeServer {
public:
	Win64NamedPipeServer(std::wstring strPipename);
	~Win64NamedPipeServer();

	virtual RESULT Initialize() override;

	virtual RESULT NamedPipeServerProcess() override;

private:
	HANDLE m_handleNamedPipe = INVALID_HANDLE_VALUE;
	int m_pipeBufferSize = DEFAULT_NAMED_PIPE_BUFFER_SIZE;

	bool m_fConnected = false;
};

#endif // ! WIN_64_NAMED_PIPE_H_