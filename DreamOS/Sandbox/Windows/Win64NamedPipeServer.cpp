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

std::wstring Win64NamedPipeServer::GetWindowsNamedPipeName() {
	std::wstring wstrWindowsName = L"\\\\.\\pipe\\" + m_strPipename;
	return wstrWindowsName;
}

RESULT Win64NamedPipeServer::Initialize() {
	RESULT r = R_PASS;

	m_handleNamedPipe = CreateNamedPipe(GetWindowsNamedPipeName().c_str(),		// pipe name 
										PIPE_ACCESS_DUPLEX,			// read/write access 
										PIPE_TYPE_MESSAGE |			// message type pipe 
										PIPE_READMODE_MESSAGE |		// message-read mode 
										PIPE_WAIT,					// blocking mode 
										PIPE_UNLIMITED_INSTANCES,	// max. instances  
										m_pipeBufferSize,			// output buffer size 
										m_pipeBufferSize,			// input buffer size 
										0,							// client time-out 
										NULL);						// default security attribute 

	CBM((m_handleNamedPipe != INVALID_HANDLE_VALUE), "Failed to create named pipe: %S Error: %d", GetWindowsNamedPipeName().c_str(), GetLastError());

Error:
	return r;
}

RESULT Win64NamedPipeServer::NamedPipeServerProcess() {
	RESULT r = R_PASS;

	HANDLE hHeap = nullptr;
	TCHAR* pchRequest = nullptr;
	TCHAR* pchReply = nullptr;

	DEBUG_LINEOUT("Pipe process started");

	// Look for connection - this call appears to be synchronous 
	m_fConnected = ConnectNamedPipe(m_handleNamedPipe, NULL); 
	if (m_fConnected == false) {
		CBM((GetLastError() != ERROR_PIPE_CONNECTED), "Pipe failed to connect");
	}
	else {
		DEBUG_LINEOUT("NamedPipeServerProcess: Client connected to pipe");
	}

	// Handle the pipe 
	hHeap = GetProcessHeap();
	pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, m_pipeBufferSize * sizeof(TCHAR));
	pchReply = (TCHAR*)HeapAlloc(hHeap, 0, m_pipeBufferSize * sizeof(TCHAR));

	DWORD cbBytesRead = 0;
	DWORD cbReplyBytes = 0;
	DWORD cbWritten = 0;

	bool fSuccess = false;

	CNM(pchRequest, "Unexpected null heap allocation for request");
	CNM(pchReply, "Unexpected null heap allocation for reply");

	// Print verbose messages. In production code, this should be for debugging only.
	DEBUG_LINEOUT("NamedPipeServerProcess: Receiving and processing messages");

	// Loop until done 
	while (m_fRunning) {

		// Read client requests from the pipe. This simplistic code only allows messages up to BUFSIZE characters in length.
		fSuccess = ReadFile(m_handleNamedPipe,					// handle to pipe 
							pchRequest,							// buffer to receive data 
							m_pipeBufferSize * sizeof(TCHAR),	// size of buffer 
							&cbBytesRead,						// number of bytes read 
							nullptr);							// not overlapped I/O 

		CBM((fSuccess), "ReadFile failed, GLE=%d", GetLastError());
		CBM((cbBytesRead != 0), "Readfile read zero bytes");

		// Process the incoming message.
		HandleRequest(pchRequest, pchReply, &cbReplyBytes);

		// Write the reply to the pipe. 
		fSuccess = WriteFile(m_handleNamedPipe,			// handle to pipe 
							 pchReply,					// buffer to write from 
							 cbReplyBytes,				// number of bytes to write 
							 &cbWritten,				// number of bytes written 
							 nullptr);					// not overlapped I/O 

		CBM((cbReplyBytes == cbWritten), "Writefile mismatch bytes written");
		CBM((fSuccess), "WriteFile failed, GLE=%d", GetLastError());
	}

Error:
	DEBUG_LINEOUT("Pipe process ended");

	// Flush the pipe to allow the client to read the pipe's contents 
	// before disconnecting. Then disconnect the pipe, and close the 
	// handle to this pipe instance. 
	if (m_handleNamedPipe != INVALID_HANDLE_VALUE) {
		FlushFileBuffers(m_handleNamedPipe);
		DisconnectNamedPipe(m_handleNamedPipe);
		CloseHandle(m_handleNamedPipe);
		m_handleNamedPipe = INVALID_HANDLE_VALUE;
	}

	if (pchReply != nullptr) {
		HeapFree(hHeap, 0, pchReply);
		pchReply = nullptr;
	}

	if (pchRequest != nullptr) {
		HeapFree(hHeap, 0, pchRequest);
		pchRequest = nullptr;
	}

	m_fRunning = false;
	m_fConnected = false;

	return r;
}

RESULT Win64NamedPipeServer::HandleRequest(LPTSTR pchRequest, LPTSTR pchReply, LPDWORD pchBytes) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Client Request String:\"%S\"", pchRequest);

	// Check the outgoing message to make sure it's not too long for the buffer.
	CRM((RESULT)(StringCchCopy(pchReply, m_pipeBufferSize, TEXT("default answer from server"))), "StringCchCopy failed, no outgoing message.\n");

	*pchBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);

	return r;

Error:
	*pchBytes = 0;
	pchReply[0] = 0;

	return r;
}