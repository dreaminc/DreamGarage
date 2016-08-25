#ifndef BOUNDING_VOLUME_H_
#define BOUNDING_VOLUME_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/BoundingVolume.h
// Bounding Volume Generic Class - this can be sub-classed by specific versions of volumes like Boxes, Spheres or otherwise
// TODO: bounding volume suggests volume as in volume in the engine, whcih is a rectilinear volume - but this is more general

#include "point.h"

class BoundingVolume {
public:
	BoundingVolume() :
		m_ptOrigin()
	{
		// Empty
	}

	BoundingVolume(point ptOrigin) :
		m_ptOrigin(ptOrigin)
	{
		// Empty
	}

	virtual bool Intersect(BoundingVolume* boundingVolume) {
		return false;
	}

protected:
	point m_ptOrigin;
};

#endif // !BOUNDING_VOLUME_H_
