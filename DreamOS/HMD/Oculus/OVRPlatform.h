#ifndef OVRPLATFORM_H_
#define OVRPLATFORM_H_

#include "./RESULT/EHM.h"

#include "External/OCULUS/OVRPlatformSDK_v1.24.0/Include/OVR_Platform.h"

class OVRPlatform {
	
public:
	OVRPlatform();
	~OVRPlatform();

	RESULT InitializePlatform();
	RESULT Update();

protected:
	const char* m_appID = "DreamOS";
	ovrPlatformInitializeResult m_initPlatformResult;

};
#endif
