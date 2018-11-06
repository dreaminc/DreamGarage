#include "Win64NamedPipeClient.h"

Win64NamedPipeClient::Win64NamedPipeClient(std::wstring wstrPipename) :
	NamedPipeClient(wstrPipename)
{
	// empty
}

Win64NamedPipeClient::~Win64NamedPipeClient() {
	// empty
}

RESULT Win64NamedPipeClient::Initialize() {
	RESULT r = R_PASS;

	m_handleNamedPipe = CreateFile(m_strPipename.c_str(),   // pipe name 
								   GENERIC_READ |  // read and write access 
								   GENERIC_WRITE,
								   0,              // no sharing 
								   NULL,           // default security attributes
								   OPEN_EXISTING,  // opens existing pipe 
								   0,              // default attributes 
								   NULL);          // no template file 

	// Break if the pipe handle is valid. 
	if (m_handleNamedPipe == INVALID_HANDLE_VALUE) {
		
		// Exit if an error other than ERROR_PIPE_BUSY occurs 
		DWORD err = GetLastError();
		CBM((err == ERROR_PIPE_BUSY), "Pipe creation faield with error 0x%x", err);

		// Pipe instances are busy, so wait for 10 seconds 
		CBM((WaitNamedPipe(m_strPipename.c_str(), 10000)), "Pipe creation timed out");
	}

	CNM(m_handleNamedPipe, "Failed to create pipe %S", m_strPipename.c_str());

Error:
	return r;
}