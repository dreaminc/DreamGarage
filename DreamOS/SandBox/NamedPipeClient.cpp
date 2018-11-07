#include "NamedPipeClient.h"

NamedPipeClient::NamedPipeClient(std::wstring strPipename) :
	m_strPipename(strPipename)
{
	// empty
}

NamedPipeClient::~NamedPipeClient() {
	// empty
}

RESULT NamedPipeClient::Start() {
	RESULT r = R_PASS;

	// Kick off thread here

	m_fRunning = true;

	CBM((m_namedPipeClientProcess.joinable() == false), "Cannot start a new named pipe client process, one already running");

	m_namedPipeClientProcess = std::thread(&NamedPipeClient::NamedPipeClientProcess, this);

	CBM((m_namedPipeClientProcess.joinable()), "Failed to start named pipe client process");

Error:
	return r;
}

RESULT NamedPipeClient::Stop() {
	RESULT r = R_PASS;

	// End thread

	CBM((m_namedPipeClientProcess.joinable()), "Cannot kill named pipe process, not running");

	m_namedPipeClientProcess.join();

	CBM((m_namedPipeClientProcess.joinable() == false), "Failed to named pipe process");

	m_fRunning = false;

Error:
	return r;
}

RESULT NamedPipeClient::RegisterMessageHandler(std::function<RESULT(void*, size_t)> fnPipeMessageHandler) {
	RESULT r = R_PASS;

	CBM((m_fnPipeMessageHandler == nullptr), "Message handler already set");
	m_fnPipeMessageHandler = fnPipeMessageHandler;

Error:
	return r;
}

RESULT NamedPipeClient::UnregisterMessageHandler() {
	RESULT r = R_PASS;

	CNM(m_fnPipeMessageHandler, "Handler not set");
	m_fnPipeMessageHandler = nullptr;

Error:
	return r;
}