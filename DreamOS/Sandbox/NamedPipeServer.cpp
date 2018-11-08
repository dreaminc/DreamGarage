#include "NamedPipeServer.h"

NamedPipeServer::NamedPipeServer(std::wstring strPipename) :
	m_strPipename(strPipename)
{
	// empty
}

NamedPipeServer::~NamedPipeServer() {
	// 
}

RESULT NamedPipeServer::Start() {
	RESULT r = R_PASS;

	// Kick off thread here

	m_fRunning = true;

	CBM((m_namedPipeServerProcess.joinable() == false), "Cannot start a new named pipe server process, one already running");

	m_namedPipeServerProcess = std::thread(&NamedPipeServer::NamedPipeServerProcess, this);

	CBM((m_namedPipeServerProcess.joinable()), "Failed to start named pipe server process");

Error:
	return r;
}

RESULT NamedPipeServer::Stop() {
	RESULT r = R_PASS;

	// End thread

	CBM((m_namedPipeServerProcess.joinable()), "Cannot kill named pipe process, not running");

	m_namedPipeServerProcess.join();

	CBM((m_namedPipeServerProcess.joinable() == false), "Failed to named pipe process");

	m_fRunning = false;

Error:
	return r;
}

RESULT NamedPipeServer::RegisterMessageHandler(std::function<RESULT(void*, size_t)> fnPipeMessageHandler) {
	RESULT r = R_PASS;

	CBM((m_fnPipeMessageHandler == nullptr), "Message handler already set");
	m_fnPipeMessageHandler = fnPipeMessageHandler;

Error:
	return r;
}

RESULT NamedPipeServer::UnregisterMessageHandler() {
	RESULT r = R_PASS;

	CNM(m_fnPipeMessageHandler, "Handler not set");
	m_fnPipeMessageHandler = nullptr;

Error:
	return r;
}