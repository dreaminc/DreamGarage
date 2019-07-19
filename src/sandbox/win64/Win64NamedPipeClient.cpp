#include "Win64NamedPipeClient.h"

Win64NamedPipeClient::Win64NamedPipeClient(std::wstring wstrPipename) :
	NamedPipeClient(wstrPipename)
{
	// empty
}

Win64NamedPipeClient::~Win64NamedPipeClient() {
	Close();
}

std::wstring GetWindowsNamedPipeClientName(std::wstring strPipename) {
	std::wstring wstrWindowsName = L"\\\\.\\pipe\\" + strPipename;
	return wstrWindowsName;
}

RESULT Win64NamedPipeClient::Close() {
	RESULT r = R_PASS;

	CBM(CancelSynchronousIo(m_namedPipeClientProcess.native_handle()), "failed to cancel sync io");
	CBM(TerminateThread(m_namedPipeClientProcess.native_handle(), 0), "failed to terminate thread");

	if (m_handleNamedPipe != INVALID_HANDLE_VALUE) {
		FlushFileBuffers(m_handleNamedPipe);
		DisconnectNamedPipe(m_handleNamedPipe);
		CloseHandle(m_handleNamedPipe);
		m_handleNamedPipe = INVALID_HANDLE_VALUE;
	}

	CR(Stop());

Error:
	return r;
}

RESULT Win64NamedPipeClient::Initialize() {
	RESULT r = R_PASS;

	//m_handleNamedPipe = CreateFile(GetWindowsNamedPipeClientName(m_strPipename).c_str(),	// pipe name 
	m_handleNamedPipe = CreateFile(L"\\\\.\\pipe\\dreamvcampipe",	// pipe name 
								   GENERIC_READ |											// read and write access 
								   GENERIC_WRITE ,
								   0,														// no sharing 
								   nullptr,													// default security attributes
								   OPEN_EXISTING ,											// opens existing pipe 
								   0,									// default attributes 
								   nullptr);												// no template file 

	// Break if the pipe handle is valid. 
	if (m_handleNamedPipe == INVALID_HANDLE_VALUE) {
		
		// Exit if an error other than ERROR_PIPE_BUSY occurs 
		DWORD err = GetLastError();
		CBM((err == ERROR_PIPE_BUSY), "Pipe creation faield with error 0x%x", err);

		// Pipe instances are busy, so wait for 10 seconds 
		CBM((WaitNamedPipe(GetWindowsNamedPipeClientName(m_strPipename).c_str(), 10000)), "Pipe creation timed out");
	}

	CNM(m_handleNamedPipe, "Failed to create pipe %S", GetWindowsNamedPipeClientName(m_strPipename).c_str());

	// Set mode of pipe
	DWORD dwMode = PIPE_READMODE_BYTE;
	bool fSuccess = SetNamedPipeHandleState(m_handleNamedPipe,		// pipe handle 
											&dwMode,				// new pipe mode 
											nullptr,				// don't set maximum bytes 
											nullptr);				// don't set maximum time 
	
	CBM(fSuccess, "SetNamedPipeHandleState failed. GLE: %d", GetLastError());

Error:
	return r;
}

RESULT Win64NamedPipeClient::NamedPipeClientProcess() {
	RESULT r = R_PASS;

	unsigned char* pBuffer = nullptr;
	size_t pBuffer_n = (size_t)m_pipeBufferSize;
	DWORD cbBytesRead = 0;

	bool fSuccess = false;

	DEBUG_LINEOUT("Pipe client process started");

	// Loop until done 

	pBuffer = new unsigned char[pBuffer_n];
	CNM(pBuffer, "Failed to allocate pipe buffer");

	while(m_fRunning) {
		
		fSuccess = ReadFile(m_handleNamedPipe,					// handle to pipe 
			(void*)pBuffer,										// buffer to receive data 
			(DWORD)pBuffer_n,									// size of buffer 
			&cbBytesRead,										// number of bytes read 
			nullptr);										// not overlapped I/O 

		// TODO: Handle this better
		if (GetLastError() == ERROR_BROKEN_PIPE) {
			DEBUG_LINEOUT("InstanceThread: client disconnected GLE: %d", GetLastError());
		}

		if (fSuccess && cbBytesRead != 0) {
			// Process the incoming message.
			if (m_fnPipeMessageHandler != nullptr) {
				CRM(m_fnPipeMessageHandler(pBuffer, (size_t)cbBytesRead), "Server pipe message handler failed");
			}
		}
		else {
			// TODO:
		}
	};

Error:
	DEBUG_LINEOUT("Pipe client process ended");

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

RESULT Win64NamedPipeClient::SendMessage(void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	DWORD cbToWrite = 0;
	DWORD cbWritten = 0;

	// Send a message to the pipe server. 

	cbToWrite = (DWORD)((pBuffer_n + 1) * sizeof(TCHAR));

	DEBUG_LINEOUT("Sending %d byte message", cbToWrite);

	bool fSuccess = WriteFile(m_handleNamedPipe,   // pipe handle 
							  pBuffer,             // message 
							  cbToWrite,           // message length 
							  &cbWritten,          // bytes written 
							  nullptr);            // not overlapped 

	CBM((fSuccess), "WriteFile to pipe failed. GLE: %d", GetLastError());

Error:
	return r;
}