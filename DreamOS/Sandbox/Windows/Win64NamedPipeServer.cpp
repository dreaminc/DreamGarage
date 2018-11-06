#include "Win64NamedPipeServer.h"

Win64NamedPipeServer::Win64NamedPipeServer(std::wstring wstrPipename) :
	NamedPipeServer(wstrPipename)
{
	// empty
}

Win64NamedPipeServer::~Win64NamedPipeServer() {
	// empty
}

RESULT Win64NamedPipeServer::Initialize() {
	RESULT r = R_PASS;

	m_handleNamedPipe = CreateNamedPipe(m_strPipename.c_str(),		// pipe name 
										PIPE_ACCESS_DUPLEX,			// read/write access 
										PIPE_TYPE_MESSAGE |			// message type pipe 
										PIPE_READMODE_MESSAGE |		// message-read mode 
										PIPE_WAIT,					// blocking mode 
										PIPE_UNLIMITED_INSTANCES,	// max. instances  
										m_pipeBufferSize,			// output buffer size 
										m_pipeBufferSize,			// input buffer size 
										0,							// client time-out 
										NULL);						// default security attribute 

	CBM((m_handleNamedPipe != INVALID_HANDLE_VALUE), "Failed to create named pipe: %S", m_strPipename.c_str());



Error:
	return r;
}

RESULT Win64NamedPipeServer::NamedPipeServerProcess() {
	RESULT r = R_PASS;

	int stayAliveCount = 0;

	while (m_fConnected == false) {
		DEBUG_LINEOUT("NamedPipeServerProcess: Waiting for connection - %d", (stayAliveCount++));

		// Look for connection
		m_fConnected = ConnectNamedPipe(m_handleNamedPipe, NULL); 
		if (m_fConnected == false) {
			CBM((GetLastError() != ERROR_PIPE_CONNECTED), "Pipe failed to connect");
		}
		else {
			DEBUG_LINEOUT("NamedPipeServerProcess: Client connected to pipe");
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	while (m_fRunning) {
		// Do stuff



	}

Error:
	if (m_handleNamedPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(m_handleNamedPipe);
		m_handleNamedPipe = INVALID_HANDLE_VALUE;
	}

	return r;
}