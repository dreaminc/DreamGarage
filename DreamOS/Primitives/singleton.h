#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "RESULT/RESULT.h"

// DREAM OS
// DreamOS/Dimension/Primitives/singleton.h
// singleton type allows a class to be a singleton using a template

// Requires the instantiation of the singleton in the source however as follows:
// Initialize and allocate the instance
// typeName* singleton<typeName>::s_pInstance = nullptr;

template <class typeName>
class singleton {
protected:
	static typeName *s_pInstance;

public:
	// Note: constructor must be public for this to work
	static typeName *instance() {
		if (!s_pInstance)
			s_pInstance = new typeName();

		return s_pInstance;
	}
};

#endif	// ! SINGLETON_H_