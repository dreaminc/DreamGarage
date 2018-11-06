#ifndef WIN_64_NAMED_PIPE_CLIENT_H_
#define WIN_64_NAMED_PIPE_CLIENT_H_


#include "RESULT/EHM.h"

// DREAM OS
// Sandbox/Windows/Win64NamedPipeClient.h
// Named pipe capability in windows 

#include "Sandbox/NamedPipeClient.h"

#include <windows.h> 

class Win64NamedPipeClient : public NamedPipeClient {
public:
	Win64NamedPipeClient(std::wstring strPipename);
	~Win64NamedPipeClient();

	virtual RESULT Initialize() override;

private:
	HANDLE m_handleNamedPipe = nullptr;
};

#endif // ! WIN_64_NAMED_PIPE_CLIENT_H_