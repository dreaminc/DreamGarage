#ifndef OVRPLATFORM_H_
#define OVRPLATFORM_H_

#include "core/ehm/EHM.h"

// Dream HMD Oculus
// dos/src/hmd/oculus/OVRPlatform.h

// TODO: This isn't really a HMD thing, more of an Oculus platform thing
// and should be put into a diff spot I think

#include "OVR_Platform.h"

#include "core/types/DObject.h"

class OVRPlatform : public DObject {
	
public:
	OVRPlatform();
	~OVRPlatform();

	RESULT InitializePlatform();
	RESULT Update();

protected:
	// TODO: get from API?
	const char* m_pszappID = "2255211697837160";

};
#endif
