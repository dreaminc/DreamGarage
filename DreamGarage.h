#ifndef DREAM_GARAGE_H_
#define DREAM_GARAGE_H_

#include "RESULT/EHM.h"
#include "Profiler/DebugConsole.h"

// DREAM GARAGE
// DreamGarage.h
// This is the Dream Garage application layer which is derived from DreamOS
// which is the interface to the engine and platform layers for the application

#include "DreamOS/DreamOS.h"

class DreamGarage : public DreamOS {
public:
	DreamGarage() {
		// empty
	}

	~DreamGarage() {
		// empty
	}

	RESULT SendHeadPosition();

	RESULT LoadScene();
	RESULT Update(void);

	// Cloud Controller
	RESULT InitializeCloudControllerCallbacks();
	RESULT HandleUpdateHeadMessage(long senderUserID, UpdateHeadMessage *pUpdateHeadMessage);
	RESULT HandleUpdateHandMessage(long senderUserID, UpdateHandMessage *pUpdateHandMessage);

private:
	sphere *m_pSphere;
	std::shared_ptr<DebugData> m_spherePosition;
};


#endif	// DREAM_GARAGE_H_