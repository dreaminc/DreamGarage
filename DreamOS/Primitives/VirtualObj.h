#ifndef VIRTUAL_OBJ_H_
#define VIRTUAL_OBJ_H_

// DREAM OS
// DreamOS/Dimension/Primitives/VirtualObj.h
// Virtual Base Object
// Virtual objects are those that do not have any actual dimensional data.  
// For example, lights and cameras may have bounds or otherwise, but no vertices 

#include "valid.h"
#include "point.h"
#include "Primitives/Types/UID.h"

class VirtualObj : public valid {
protected:
	point m_ptOrigin;   // origin

public:
	VirtualObj() :
		m_ptOrigin()
	{
		/* stub */
	}

	~VirtualObj() {
		// Empty Stub
	}

private:
	UID m_uid;
};

#endif // ! VIRTUAL_OBJ_H_