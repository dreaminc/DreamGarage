#include "WebRTCImp.h"

#include "webrtc/base/ssladapter.h"
#include "webrtc/base/win32socketinit.h"

#include "WebRTCClient.h"
#include "WebRTCConductor.h"

#include "webrtc/base/arraysize.h"

WebRTCImp::WebRTCImp() :
	CloudImp()
{
	// empty
}

WebRTCImp::~WebRTCImp() {
	rtc::CleanupSSL();
}

// CloudImp Interface
RESULT WebRTCImp::Initialize() {
	RESULT r = R_PASS;

	rtc::scoped_refptr<WebRTCConductor> pWebRTCConductor = nullptr;

	//m_pWin32thread = std::shared_ptr<rtc::Win32Thread>();
	//rtc::Win32Thread Win32thread;

	rtc::EnsureWinsockInit();
	//rtc::ThreadManager::Instance()->SetCurrentThread(m_pWin32thread.get());
	//rtc::ThreadManager::Instance()->SetCurrentThread(&Win32thread);
	rtc::ThreadManager::Instance()->SetCurrentThread(&m_Win32thread);
	rtc::InitializeSSL();

	m_pWebRTCClient = std::make_shared<WebRTCClient>(this);
	CN(m_pWebRTCClient);

	pWebRTCConductor = rtc::scoped_refptr<WebRTCConductor>(new rtc::RefCountedObject<WebRTCConductor>(m_pWebRTCClient.get(), this));
	m_pWebRTCConductor = std::shared_ptr<WebRTCConductor>(pWebRTCConductor.get());

Error:
	return r;
}

RESULT WebRTCImp::CreateNewURLRequest(std::wstring& strURL) {
	return R_NOT_IMPLEMENTED;
}

// Will simply update the message loop as needed
RESULT WebRTCImp::Update() {
	return R_NOT_IMPLEMENTED;
}

// WebRTC Specific

RESULT WebRTCImp::QueueUIThreadCallback(int msg_id, void* data) {
	RESULT r = R_PASS;
	
	CB(::PostThreadMessage(m_UIThreadID, UI_THREAD_CALLBACK, static_cast<WPARAM>(msg_id), reinterpret_cast<LPARAM>(data)));

Error:
	return r;
}

// Functionality
RESULT WebRTCImp::StartLogin(const std::string& strServer, int port) {
	RESULT r = R_PASS;

	CB((m_pWebRTCClient->IsConnected() != true));

	m_strServer = strServer;
	m_pWebRTCClient->Connect(strServer, port, GetPeerName());

Error:
	return r;
}

// Utilities
std::string WebRTCImp::GetPeerName() {
	char computer_name[256];

	std::string ret(GetEnvVarOrDefault("USERNAME", "user"));
	ret += '@';
	if (gethostname(computer_name, arraysize(computer_name)) == 0) {
		ret += computer_name;
	}
	else {
		ret += "host";
	}

	return ret;
}

std::string WebRTCImp::GetEnvVarOrDefault(const char* env_var_name, const char* default_value) {
	std::string value;
	const char* env_var = getenv(env_var_name);

	if (env_var)
		value = env_var;

	if (value.empty())
		value = default_value;

	return value;
}

RESULT WebRTCImp::OnSignedIn() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnSignedIn");

Error:
	return r;
}

RESULT WebRTCImp::OnDisconnected() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnDisconnected");

Error:
	return r;
}

RESULT WebRTCImp::OnPeerConnected(int id, const std::string& name) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnPeerConnected");

Error:
	return r;
}

RESULT WebRTCImp::OnPeerDisconnected(int peer_id) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnPeerDisconnected");

Error:
	return r;
}

RESULT WebRTCImp::OnMessageFromPeer(int peer_id, const std::string& message) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnMessageFromPeer");

Error:
	return r;
}

RESULT WebRTCImp::OnMessageSent(int err) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WebRTCImp: OnMessageSent");

Error:
	return r;
}

RESULT WebRTCImp::OnServerConnectionFailure() {
	DEBUG_LINEOUT("WebRTCImp Error: Failed to connect to %s", m_strServer.c_str());
	return R_PASS;
}