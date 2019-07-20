#ifndef OBJECT_H_
#define OBJECT_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"

// DREAM OS
// DreamOS/Core/Types/Factory.h
// The base Factory type - for you know, factories and such
// TODO: These base types should also be used to track all of the factories of the platform

class Object {
public:
	Object() {
		// empty stub
	}

	~Object() {
		// empty stub
	}

private:
	UID m_uid;
};

#else
	// Throw error for these core types
	#error OBJECT_H_ include collision
#endif // ! OBJECT_H_