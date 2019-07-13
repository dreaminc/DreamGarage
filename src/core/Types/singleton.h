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
public:
	static typeName *s_pInstance;

public:
	// Note: constructor must be public for this to work
	static typeName *instance() {
		RESULT r = R_PASS;

		if (!s_pInstance) {
			s_pInstance = new typeName();
			CN(s_pInstance);

			// This allows the singleton to run an initialization function that
			// can fail (unlike the constructor)
			CR(s_pInstance->InitializeSingleton());
		}

	// Success:
		return s_pInstance;

	Error:
		if (s_pInstance != nullptr) {
			delete s_pInstance;
			s_pInstance = nullptr;
		}

		return nullptr;
	}

protected:
	virtual RESULT InitializeSingleton() { return R_NOT_IMPLEMENTED; }
};

#endif	// ! SINGLETON_H_