#ifndef WIN_64_NAMED_PIPE_H_
#define WIN_64_NAMED_PIPE_H_


#include "RESULT/EHM.h"

// DREAM OS
// Sandbox/Windows/Win64NamedPipe.h
// Named pipe capability in windows 

#include "Sandbox/NamedPipeServer.h"

#include <windows.h> 

#define DEFAULT_NAMED_PIPE_SERVER_BUFFER_SIZE 1280 * 720 * 4
#define DEFAULT_NAMED_PIPE_TIMEOUT 0

class Win64NamedPipeServer;

class Win64NamedPipeConnection {
	friend class Win64NamedPipeServer;
public:
	Win64NamedPipeConnection(Win64NamedPipeServer *pParentServer, HANDLE handleNamedPipe, LPOVERLAPPED pOverlapped, int connectionID);
	~Win64NamedPipeConnection();

	RESULT ClosePipe();

protected:
	RESULT ConnectPipe();
	bool IsConnected() { return m_fConnected; }

	bool m_fConnected = false;
	bool m_fPendingConnection = false;
	int m_connectionID = 0;
	HANDLE m_handleNamedPipe = INVALID_HANDLE_VALUE;
	DWORD m_cbToWrite = 0;
	DWORD m_cbWritten = 0;

private:
	Win64NamedPipeServer *m_pParentServer = nullptr;
	
	
	int m_pipeBufferSize = DEFAULT_NAMED_PIPE_SERVER_BUFFER_SIZE;
	
	
	LPOVERLAPPED m_pOverlapped = nullptr;
	

};

class Win64NamedPipeServer : public NamedPipeServer {

public:
	Win64NamedPipeServer(std::wstring strPipename);
	~Win64NamedPipeServer();

	virtual RESULT Initialize() override;
	virtual RESULT Close() override;

	virtual RESULT NamedPipeServerProcess() override;
	virtual RESULT SendMessage(void *pBuffer, size_t pBuffer_n) override;

	virtual RESULT ClearConnections() override;

private:
	RESULT AllocateAndConnectPendingConnectionInstance();
	RESULT AddPendingConnectionInstanceAndAllocateNew();

private:
	std::vector<Win64NamedPipeConnection*> m_clientConnections;

	Win64NamedPipeConnection *m_pPendingConnection = nullptr;

	HANDLE m_handleNamedPipe = INVALID_HANDLE_VALUE;
	int m_pipeBufferSize = DEFAULT_NAMED_PIPE_SERVER_BUFFER_SIZE;
	int m_msTimeout = DEFAULT_NAMED_PIPE_TIMEOUT;
	OVERLAPPED m_overlapped; 
	HANDLE m_hConnectEvent = nullptr;
};



#endif // ! WIN_64_NAMED_PIPE_H_