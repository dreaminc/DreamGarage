#ifndef WEB_RTC_ICE_CONNECTION_H_
#define WEB_RTC_ICE_CONNECTION_H_

#include "RESULT/EHM.h"

// WebRTC Ice Connection
// Right now a shell class, but more functionality could be added here to manipulate the ICE connection state
// and WebRTC stuff

class WebRTCIceConnection {
public:
	enum class state {
		NEW,
		CHECKING,
		CONNECTED,
		COMPLETED,
		FAILED,
		DISCONNECTED,
		CLOSED,
		MAX,
		INVALID
	};

private:
	// empty
};

#endif // ! WEB_RTC_ICE_CONNECTION_H_
