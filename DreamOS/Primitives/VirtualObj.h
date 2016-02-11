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

	// This should also work with vector
	RESULT translate(matrix <point_precision, 4, 1> v) {
		m_ptOrigin.translate(v);
		return R_PASS;
	}

	RESULT translate(point_precision x, point_precision y, point_precision z) {
		m_ptOrigin.translate(x, y, z);
		return R_PASS;
	}

	RESULT MoveTo(point p) {
		m_ptOrigin = p;
	}

	RESULT MoveTo(point_precision x, point_precision y, point_precision z) {
		m_ptOrigin.x() = x;
		m_ptOrigin.y() = y;
		m_ptOrigin.z() = z;

		return R_PASS;
	}

private:
	UID m_uid;
};

#endif // ! VIRTUAL_OBJ_H_