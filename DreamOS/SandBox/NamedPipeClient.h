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

protected:
	std::wstring m_strPipename;
};

#endif // ! DREAM_NAMED_PIPE_CLIENT_H_