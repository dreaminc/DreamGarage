#include "OVRPlatform.h"
#include "OVR_Platform.h"

OVRPlatform::OVRPlatform() 
{
	// empty stub
}

OVRPlatform::~OVRPlatform() 
{
	// empty stub
}

RESULT OVRPlatform::InitializePlatform() {
	RESULT r = R_PASS;
	
	// Initialization call
	// Exit if failed.  Initialization failed which means either the oculus service isn’t on the machine or they’ve hacked their DLL
	CBM(ovr_PlatformInitializeWindows(m_pszappID) == ovrPlatformInitialize_Success, "Failed to initialize Oculus Platform, check if service is running");
	
#ifdef OCULUS_PRODUCTION_BUILD
	// Fails/succeeds asynchronously - will send a message to handle in Update()
	ovr_Entitlement_GetIsViewerEntitled();
#endif

Error:
	return r;
}

RESULT OVRPlatform::Update() {
	RESULT r = R_PASS;

	ovrMessageHandle ovrMessage;

	while ((ovrMessage = ovr_PopMessage()) != nullptr) {

		switch (ovr_Message_GetType(ovrMessage)) {
		
		case ovrMessage_PlatformInitializeWindowsAsynchronous: {
			CBM(!ovr_Message_IsError(ovrMessage), "Failed to initialize Oculus Platform");
		} break;

		case ovrMessage_Entitlement_GetIsViewerEntitled: {
			CBM(!ovr_Message_IsError(ovrMessage), "User failed Oculus entitlement check");
		} break;

		default: {
			CR(r);
		} break;
		}
	}

Error:
	return r;
}