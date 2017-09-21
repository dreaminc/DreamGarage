#ifndef D_OBJECT_H_
#define D_OBJECT_H_

#include "RESULT/RESULT.h"

// DREAM OS
// DreamOS/Primitives/DObject.h
// The base level Dream Object to be managed by the engine
// comparable to NSObject in Mac land

#include "Primitives/valid.h"
#include "Primitives/Types/UID.h"

class DObject : public valid {
public:
	DObject() {
		// empty
	}

	~DObject() {
		// empty
	}

private:
	UID m_uid;
};


#endif	// ! D_OBJECT_H_