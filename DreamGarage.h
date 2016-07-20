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

	RESULT LoadScene();
	RESULT Update(void);

private:
	sphere *m_pSphere;
	std::shared_ptr<DebugData> p;
};


#endif	// DREAM_GARAGE_H_