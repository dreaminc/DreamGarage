#ifndef OVRPLATFORM_H_
#define OVRPLATFORM_H_

#include "./RESULT/EHM.h"

#include "OVR_Platform.h"

class OVRPlatform {
	
public:
	OVRPlatform();
	~OVRPlatform();

	RESULT InitializePlatform();
	RESULT Update();

protected:

	// TODO: get from API?
	const char* m_appID = "2255211697837160";

};
#endif
