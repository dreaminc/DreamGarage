#include "WebRTCClient.h"

WebRTCClient::WebRTCClient() :
	m_WebRTCID(-1),
	m_WebRTCState(UNINITIALIZED)
{
	// empty
}

WebRTCClient::~WebRTCClient() {
	// empty
}


void WebRTCClient::OnMessage(rtc::Message* msg) {

}