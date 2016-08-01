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

	//rtc::scoped_refptr<WebRTCConductor> pWebRTCConductor = nullptr;

	m_pWin32thread = std::shared_ptr<rtc::Win32Thread>();

	rtc::EnsureWinsockInit();
	rtc::ThreadManager::Instance()->SetCurrentThread(m_pWin32thread.get());
	rtc::InitializeSSL();

	m_pWebRTCClient = std::shared_ptr<WebRTCClient>(new WebRTCClient(std::make_shared<WebRTCImp>(this)));
	CN(m_pWebRTCClient);

	//rtc::scoped_refptr<WebRTCConductor> pWebRTCConductor(new rtc::RefCountedObject<WebRTCConductor>(m_pWebRTCClient, this));
	m_pWebRTCConductor = std::make_shared<WebRTCConductor>(m_pWebRTCClient, this);

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

RESULT WebRTCImp::OnServerConnectionFailure() {
	DEBUG_LINEOUT("WebRTCImp Error: Failed to connect to %s", m_strServer.c_str());
	return R_PASS;
}