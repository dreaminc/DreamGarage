#ifndef WIN_64_NAMED_PIPE_CLIENT_H_
#define WIN_64_NAMED_PIPE_CLIENT_H_


#include "RESULT/EHM.h"

// DREAM OS
// Sandbox/Windows/Win64NamedPipeClient.h
// Named pipe capability in windows 

#include "Sandbox/NamedPipeClient.h"

#include <windows.h> 

#define DEFAULT_NAMED_PIPE_CLIENT_BUFFER_SIZE 512

class Win64NamedPipeClient : public NamedPipeClient {
public:
	Win64NamedPipeClient(std::wstring strPipename);
	~Win64NamedPipeClient();

	virtual RESULT Close() override;
	virtual RESULT Initialize() override;

	virtual RESULT NamedPipeClientProcess() override;
	virtual RESULT SendMessage(void *pBuffer, size_t pBuffer_n) override;

private:
	HANDLE m_handleNamedPipe = nullptr;
	int m_pipeBufferSize = DEFAULT_NAMED_PIPE_CLIENT_BUFFER_SIZE;

	bool m_fConnected = false;
};

#endif // ! WIN_64_NAMED_PIPE_CLIENT_H_