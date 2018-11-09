#include "Win64NamedPipeServer.h"

#include <strsafe.h>

Win64NamedPipeConnection::Win64NamedPipeConnection(Win64NamedPipeServer *pParentServer, HANDLE handleNamedPipe, LPOVERLAPPED pOverlapped, int connectionID) :
	m_pParentServer(pParentServer),
	m_handleNamedPipe(handleNamedPipe),
	m_pOverlapped(pOverlapped),
	m_connectionID(connectionID)
{
	// empty
}

Win64NamedPipeConnection::~Win64NamedPipeConnection() {
	ClosePipe();
}

RESULT Win64NamedPipeConnection::ConnectPipe() {
	RESULT r = R_PASS;

	CN(m_handleNamedPipe);

	// Start an overlapped connection for this pipe instance. 
	m_fConnected = ConnectNamedPipe(m_handleNamedPipe, m_pOverlapped);

	// Overlapped ConnectNamedPipe should return zero. 
	if (m_fConnected == false) {
		switch (GetLastError()) {
			// The overlapped connection in progress. 
			case ERROR_IO_PENDING: {
				m_fPendingConnection = true;
			} break;

			// Client is already connected, so signal an event. 
			case ERROR_PIPE_CONNECTED: {
				if (SetEvent(m_pOverlapped->hEvent)) {
					DEBUG_LINEOUT("Connection %d connected!", m_connectionID);
					m_fConnected = true;
				}
			} break;

			// If an error occurs during the connect operation... 
			default: {
				CBM((false), "ConnectNamedPipe failed with %d.\n", GetLastError());
			} break;
		}
	}
	else {
		DEBUG_LINEOUT("Connection %d connected!", m_connectionID);
	}

Error:
	return r;
}

RESULT Win64NamedPipeConnection::ClosePipe() {
	RESULT r = R_PASS;

	// Flush the pipe to allow the client to read the pipe's contents  before disconnecting. 
	// Then disconnect the pipe, and close the handle to this pipe instance. 

	if (m_handleNamedPipe != INVALID_HANDLE_VALUE && m_handleNamedPipe != nullptr) {
		FlushFileBuffers(m_handleNamedPipe);
		DisconnectNamedPipe(m_handleNamedPipe);
		CloseHandle(m_handleNamedPipe);

		m_handleNamedPipe = INVALID_HANDLE_VALUE;
	}

Error:
	return r;
}

Win64NamedPipeServer::Win64NamedPipeServer(std::wstring wstrPipename) :
	NamedPipeServer(wstrPipename)
{
	// empty
}

Win64NamedPipeServer::~Win64NamedPipeServer() {
	Close();
}

RESULT Win64NamedPipeServer::Close() {
	RESULT r = R_PASS;

	m_clientConnections.clear();

Error:
	return r;
}


std::wstring GetWindowsNamedPipeServerName(std::wstring strPipename) {
	std::wstring wstrWindowsName = L"\\\\.\\pipe\\" + strPipename;
	return wstrWindowsName;
}

RESULT Win64NamedPipeServer::Initialize() {
	RESULT r = R_PASS;

	/*
	m_handleNamedPipe = CreateNamedPipe(GetWindowsNamedPipeServerName(m_strPipename).c_str(),		// pipe name 
										PIPE_ACCESS_DUPLEX |												// read/write access 
										FILE_FLAG_OVERLAPPED,												// overlapped mode 
										PIPE_TYPE_MESSAGE |													// message-type pipe 
										PIPE_READMODE_MESSAGE |												// message read mode 
										PIPE_WAIT,															// blocking mode 
										PIPE_UNLIMITED_INSTANCES,											// unlimited instances 
										m_pipeBufferSize,											// output buffer size 
										m_pipeBufferSize,											// input buffer size 
										m_msTimeout,															// client time-out 
										nullptr);													// default security attribute 
	
	CBM((m_handleNamedPipe != INVALID_HANDLE_VALUE), "Failed to create named pipe: %S Error: %d", GetWindowsNamedPipeServerName(m_strPipename).c_str(), GetLastError());
	
	DEBUG_LINEOUT("Created %S pipe", GetWindowsNamedPipeServerName(m_strPipename).c_str());
	*/

	// Create one event object for the connect operation. 
	m_hConnectEvent = CreateEvent(
		nullptr,    // default security attribute
		true,		// manual reset event 
		true,		// initial state = signaled 
		nullptr);   // unnamed event object 

	CNM(m_hConnectEvent, "CreateEvent failed with %d", GetLastError());
	m_overlapped.hEvent = m_hConnectEvent;

	// This is supposed to fail if there are no pending connections
	// TODO: Handle this more eloquently 
	AllocateAndConnectPendingConnectionInstance();

Error:
	return r;
}

// CreateAndConnectInstance(LPOVERLAPPED) 
// This function creates a pipe instance and connects to the client. 
// It returns TRUE if the connect operation is pending, and FALSE if 
// the connection has been completed. 

RESULT Win64NamedPipeServer::AllocateAndConnectPendingConnectionInstance() {
	RESULT r = R_PASS;

	static int connectionID = 0;

	CBM((m_pPendingConnection == nullptr), "Allocated connection already pending");

	DEBUG_LINEOUT("Allocating new connection id: %d", connectionID);

	// Create the pipe
	HANDLE handlePipe = CreateNamedPipe(
		//GetWindowsNamedPipeServerName(m_strPipename).c_str(),				// pipe name 
		L"\\\\.\\pipe\\dreamvcampipe",	// pipe name 
		PIPE_ACCESS_DUPLEX |												// read/write access 
		FILE_FLAG_OVERLAPPED,												// overlapped mode 
		PIPE_TYPE_MESSAGE |													// message-type pipe 
		PIPE_READMODE_MESSAGE |												// message read mode 
		PIPE_WAIT,															// blocking mode 
		PIPE_UNLIMITED_INSTANCES,											// unlimited instances 
		m_pipeBufferSize,															// output buffer size 
		m_pipeBufferSize,															// input buffer size 
		m_msTimeout,															// client time-out 
		nullptr);															// default security attributes

	CBM((handlePipe != INVALID_HANDLE_VALUE), "CreateNamedPipe failed with %d.\n", GetLastError());

	m_pPendingConnection = new Win64NamedPipeConnection(this, handlePipe, &m_overlapped, connectionID++);
	CN(m_pPendingConnection);
	
	// Connect the pipe
	CRM(m_pPendingConnection->ConnectPipe(), "Failed to connect pipe");

Error:
	return r;
}


RESULT Win64NamedPipeServer::AddPendingConnectionInstanceAndAllocateNew() {
	RESULT r = R_PASS;

	CN(m_pPendingConnection != nullptr);

	m_pPendingConnection->m_fConnected = true;
	m_pPendingConnection->m_fPendingConnection = false;
	DEBUG_LINEOUT("Connection %d connected!", m_pPendingConnection->m_connectionID);

	m_clientConnections.push_back(m_pPendingConnection);
	m_pPendingConnection = nullptr;

	// This is supposed to fail if there are no pending connections
	// TODO: Handle this more eloquently 
	AllocateAndConnectPendingConnectionInstance();

Error:
	return r;
}

RESULT Win64NamedPipeServer::NamedPipeServerProcess() {
	RESULT r = R_PASS;
	
	DWORD dwEventWait;
	bool fSuccess = false;

	// Call a subroutine to create one instance, and wait for 
	// the client to connect. 

	DEBUG_LINEOUT("Starting server...");

	while (m_fRunning)  {

		// Wait for a client to connect, or for a read or write 
		// operation to be completed, which causes a completion 
		// routine to be queued for execution. 

		dwEventWait = WaitForSingleObjectEx(
			m_hConnectEvent,  // event object to wait for 
			INFINITE,       // waits indefinitely 
			true);          // alert-able wait enabled 

		switch (dwEventWait) { 

			// The wait conditions are satisfied by a completed connect 
			// operation. 
			case 0: {

				// If an operation is pending, get the result of the 
				// connect operation. 

				//if (fPendingIO) {
				//	fSuccess = GetOverlappedResult(
				//		m_handleNamedPipe,		// pipe handle 
				//		&overlappedConnect,				// OVERLAPPED structure 
				//		&cbRet,					// bytes transferred 
				//		false);					// does not wait 
				//
				//	CBM((fSuccess), "ConnectNamedPipe (%d)\n", GetLastError());
				//}

				// Allocate storage for next instance. 
				AddPendingConnectionInstanceAndAllocateNew();
			} break;

			// The wait is satisfied by a completed read or write 
			// operation. This allows the system to execute the 
			// completion routine. 
			case WAIT_IO_COMPLETION: {
				//
			} break;

			// An error occurred in the wait function. 
			default: {
				CBM((false), "WaitForSingleObjectEx (%d)\n", GetLastError());
			} break;
		}
	}

Error:
	DEBUG_LINEOUT("Pipe server process ended");

	//if (pBuffer != nullptr) {
	//	delete[] pBuffer;
	//	pBuffer = nullptr;
	//}

	m_fRunning = false;
	//m_fConnected = false;

	return r;
}

RESULT Win64NamedPipeServer::SendMessage(void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	// Send a message to the pipe client 

	for (auto &pClientConnection : m_clientConnections) {
			
		pClientConnection->m_cbToWrite = (DWORD)((pBuffer_n + 1) * sizeof(TCHAR));

		if (pClientConnection->m_fConnected) {
			bool fSuccess = WriteFile(
				pClientConnection->m_handleNamedPipe,   // pipe handle 
				pBuffer,								// message 
				pClientConnection->m_cbToWrite,         // message length 
				&pClientConnection->m_cbWritten,        // bytes written 
				nullptr);								// not overlapped 

			if (fSuccess == false) {
				DWORD err = GetLastError();

				if (err == ERROR_PIPE_LISTENING) {
					CBM((false), "WriteFile warning waiting for connection");
				}
				else if (err == ERROR_NO_DATA) {
					pClientConnection->m_fConnected = false;
					DEBUG_LINEOUT("Client connection %d closed", pClientConnection->m_connectionID);
				}
				else {
					CBM((false), "WriteFile failed with GLE: %d", (int)err);
				}
			}
			else {
				CBM((pClientConnection->m_cbToWrite == pClientConnection->m_cbWritten), "Writefile mismatch bytes written");
			}
		}
	}

Error:
	return r;
}