#ifndef MANAGER_H_
#define MANAGER_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/types/Manager.h

// The virtual manager interface object which 
// effectively manages things

#include "core/types/DObject.h"

// TODO: Could squash this with a template 
// TODO: Is this a module?
class Manager : public DObject {
public:
	Manager() {
		// empty
	}

	~Manager() {
		// empty
	}

	// Initialize function called when manager is first created by factory
	virtual RESULT Initialize() = 0;

	// Update function called to give the manager a chance to update itself
	// Unless this is handled in a thread
	virtual RESULT Update() = 0;


	// Shutdown the manager
	virtual RESULT Shutdown() = 0;
};

#endif	// ! MANAGER_H_