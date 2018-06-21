#include "OVRPlatform.h"
#include "External/OCULUS/OVRPlatformSDK_v1.24.0/Include/OVR_Platform.h"

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
	
	ovr_PlatformInitializeWindowsAsynchronous(m_appID, &m_initPlatformResult);

	// Initialization call
	if (m_initPlatformResult != ovrPlatformInitialize_Success) {
		// Exit.  Initialization failed which means either the oculus service isn’t on the machine or they’ve hacked their DLL
		CBM(m_initPlatformResult == ovrPlatformInitialize_Success, "Failed to initialize Oculus Platform, check if service is running");
	}
	ovr_Entitlement_GetIsViewerEntitled();

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

			if(!ovr_Message_IsError(ovrMessage)) {
				// User is entitled.  Continue with normal game behaviour
				// Is what they say but we're not a "game" >:O
				CR(r);
			}
			else {
				// Not entitled, can't launch full app, but can handle elegantly... probably?
				// We'll want to bubble up an event for this, fail for now
				CBM(!ovr_Message_IsError(ovrMessage), "User failed Oculus entitlement check");
			}
		} break;

		default: {
			CR(r);
		} break;
		}
	}

Error:
	return r;
}