#ifndef RAY_H_
#define RAY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/ray.h
// Ray primitive class
// A ray is a point and corresponding vector

#include "point.h"
#include "vector.h"

class ray {
public:
	ray() :
		m_ptOrigin(),
		m_vDirection()
	{
		// Empty
	}

	ray(point ptOrigin, vector vDirection) :
		m_ptOrigin(ptOrigin),
		m_vDirection(vDirection)
	{
		// Empty
	}

	friend class BoundingVolume;

	vector GetVector() {
		return m_vDirection;
	}

	point GetOrigin() {
		return m_ptOrigin;
	}

	inline point &origin() { return m_ptOrigin; }
	inline vector &direction() { return m_vDirection; }

protected:
	point m_ptOrigin;
	vector m_vDirection;
};

#endif // !RAY_H_
