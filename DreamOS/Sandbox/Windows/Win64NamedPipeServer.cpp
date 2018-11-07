#include "Win64NamedPipeServer.h"

#include <strsafe.h>

Win64NamedPipeServer::Win64NamedPipeServer(std::wstring wstrPipename) :
	NamedPipeServer(wstrPipename)
{
	// empty
}

Win64NamedPipeServer::~Win64NamedPipeServer() {
	// empty
}

std::wstring GetWindowsNamedPipeServerName(std::wstring strPipename) {
	std::wstring wstrWindowsName = L"\\\\.\\pipe\\" + strPipename;
	return wstrWindowsName;
}

RESULT Win64NamedPipeServer::Initialize() {
	RESULT r = R_PASS;

	m_handleNamedPipe = CreateNamedPipe(GetWindowsNamedPipeServerName(m_strPipename).c_str(),		// pipe name 
										PIPE_ACCESS_DUPLEX,											// read/write access 
										PIPE_TYPE_BYTE |											// message type pipe 
										PIPE_READMODE_BYTE |										// message-read mode 
										PIPE_WAIT,													// blocking mode 
										PIPE_UNLIMITED_INSTANCES,									// max. instances  
										m_pipeBufferSize,											// output buffer size 
										m_pipeBufferSize,											// input buffer size 
										0,															// client time-out 
										nullptr);													// default security attribute 

	CBM((m_handleNamedPipe != INVALID_HANDLE_VALUE), "Failed to create named pipe: %S Error: %d", GetWindowsNamedPipeServerName(m_strPipename).c_str(), GetLastError());

Error:
	return r;
}

RESULT Win64NamedPipeServer::NamedPipeServerProcess() {
	RESULT r = R_PASS;

	unsigned char* pBuffer = nullptr;
	size_t pBuffer_n = (size_t)m_pipeBufferSize;
	DWORD cbBytesRead = 0;

	bool fSuccess = false;

	DEBUG_LINEOUT("Pipe server process started");

	// Look for connection - this call appears to be synchronous 
	m_fConnected = ConnectNamedPipe(m_handleNamedPipe, nullptr); 

	if (m_fConnected == false) {
		// Connection occurred between CreateNamedPipe and ConnectNamedPipe
		// so connection is still valid
		CBM((GetLastError() == ERROR_PIPE_CONNECTED), "Pipe failed to connect error: %d", GetLastError());
		m_fConnected = true;
	}
	else {
		DEBUG_LINEOUT("NamedPipeServerProcess: Client connected to pipe");
	}

	pBuffer = new unsigned char[pBuffer_n];
	CNM(pBuffer, "Failed to allocate pipe buffer");

	DEBUG_LINEOUT("NamedPipeServerProcess: Receiving and processing messages");

	// Loop until done 
	while (m_fRunning) {

		fSuccess = ReadFile(m_handleNamedPipe,					// handle to pipe 
							(void*)pBuffer,						// buffer to receive data 
							(DWORD)pBuffer_n,					// size of buffer 
							&cbBytesRead,						// number of bytes read 
							nullptr);							// not overlapped I/O 

		// TODO: Handle this better
		if (GetLastError() == ERROR_BROKEN_PIPE) {
			DEBUG_LINEOUT("InstanceThread: client disconnected GLE: %d", GetLastError());
		}

		CBM((fSuccess), "ReadFile failed, GLE=%d", GetLastError());
		CBM((cbBytesRead != 0), "Readfile read zero bytes");

		// Process the incoming message.
		if (m_fnPipeMessageHandler != nullptr) {
			CRM(m_fnPipeMessageHandler(pBuffer, (size_t)cbBytesRead), "Server pipe message handler failed");
		}
	}

Error:
	DEBUG_LINEOUT("Pipe server process ended");

	// Flush the pipe to allow the client to read the pipe's contents 
	// before disconnecting. Then disconnect the pipe, and close the 
	// handle to this pipe instance. 
	if (m_handleNamedPipe != INVALID_HANDLE_VALUE) {
		FlushFileBuffers(m_handleNamedPipe);
		DisconnectNamedPipe(m_handleNamedPipe);
		CloseHandle(m_handleNamedPipe);
		m_handleNamedPipe = INVALID_HANDLE_VALUE;
	}

	if (pBuffer != nullptr) {
		delete[] pBuffer;
		pBuffer = nullptr;
	}

	m_fRunning = false;
	m_fConnected = false;

	return r;
}

RESULT Win64NamedPipeServer::SendMessage(void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	DWORD cbToWrite = 0;
	DWORD cbWritten = 0;

	// Send a message to the pipe client 

	cbToWrite = (DWORD)((pBuffer_n + 1) * sizeof(TCHAR));

	// Write the reply to the pipe. 
	bool fSuccess = WriteFile(m_handleNamedPipe,   // pipe handle 
		pBuffer,             // message 
		cbToWrite,           // message length 
		&cbWritten,          // bytes written 
		nullptr);            // not overlapped 

	CBM((fSuccess), "WriteFile failed, GLE=%d", GetLastError());

	CBM((cbToWrite == cbWritten), "Writefile mismatch bytes written");

Error:
	return r;
}