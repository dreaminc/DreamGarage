#ifndef PROXY_H_
#define PROXY_H_

#include "core/ehm/EHM.h"

// DREAM OS
// DreamOS/Primitives/Proxy.h
// The Proxy Object

template <class T>
class Proxy {
public:
	virtual T* GetProxy() = 0;
};

#endif // ! PROXY_H_

