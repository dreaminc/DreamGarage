#ifndef WIN_64_NAMED_PIPE_CLIENT_H_
#define WIN_64_NAMED_PIPE_CLIENT_H_

#include "core/ehm/EHM.h"

// Dream Sandbox Win64
// dos/src/sandbox/win64/Win64NamedPipeClient.h

// Named pipe capability in windows 

#include <windows.h> 

#include "sandbox/NamedPipeClient.h"

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