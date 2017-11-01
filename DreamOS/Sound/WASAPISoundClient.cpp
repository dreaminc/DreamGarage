#include "WASAPISoundClient.h"

WASAPISoundClient::WASAPISoundClient() {
	// empty
}

WASAPISoundClient::~WASAPISoundClient() {\
	// empty
}

RESULT WASAPISoundClient::Initialize() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Initializing WASAPI Sound Client");

	CR(r);

Error:
	return r;
}