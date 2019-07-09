#ifndef FACTORY_H_
#define FACTORY_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Object.h"

// DREAM OS
// DreamOS/Core/Types/Factory.h
// The base Factory type - for you know, factories and such
// TODO: These base types should also be used to track all of the factories of the platform

class Factory  : public Object {
public:
	Factory() {
		// empty stub
	}

	~Factory() {
		// empty stub
	}
};

#endif // ! FACTORY_H_