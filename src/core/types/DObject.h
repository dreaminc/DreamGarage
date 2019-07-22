#ifndef D_OBJECT_H_
#define D_OBJECT_H_

#include "core/ehm/RESULT.h"

// DREAM OS
// dos/src/core/types/DObject.h

// The base level Dream Object to be managed by the engine
// comparable to NSObject in Mac land

#include "core/types/valid.h"
#include "core/types/UID.h"

class DObject : 
	public valid
{
public:
	DObject() {
		// empty
	}

	~DObject() {
		// empty
	}

public:
	UID getID() { 
		return m_uid; 
	}

	UINT64 GetUIDValue() {
		return m_uid.GetID();
	}

private:
	UID m_uid;
};


#endif	// ! D_OBJECT_H_