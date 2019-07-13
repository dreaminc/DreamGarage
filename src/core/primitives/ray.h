#ifndef RAY_H_
#define RAY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/ray.h
// Ray primitive class
// A ray is a point and corresponding vector

#include "point.h"
#include "vector.h"
#include "VirtualObj.h"

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

	ray(const ray &r) :
		m_ptOrigin(r.m_ptOrigin),
		m_vDirection(r.m_vDirection)
	{
		// Empty
	}

	friend class BoundingVolume;

	vector GetVector() const {
		return m_vDirection;
	}

	RESULT SetDirectionVector(const vector &vDirection) {
		m_vDirection = vDirection;
		return R_PASS;
	}

	point GetOrigin() const {
		return m_ptOrigin;
	}

	RESULT Print(const char *pszName = "ray") {
		DEBUG_LINEOUT("%s(pt(%f, %f, %f), v(%f, %f, %f))",
			pszName,
			m_ptOrigin.x(), m_ptOrigin.y(), m_ptOrigin.z(),
			m_vDirection.x(), m_vDirection.y(), m_vDirection.z()
		);

		return R_PASS;
	}

	inline point &ptOrigin() { return m_ptOrigin; }
	inline vector &vDirection() { return m_vDirection; }

	template <typename TMatrix>
	friend ray operator*(matrix<TMatrix, 4, 4>& lhs, const ray& rhs) {
		ray retRay;

		retRay.ptOrigin() = lhs * const_cast<ray&>(rhs).ptOrigin();
		retRay.vDirection() = lhs * const_cast<ray&>(rhs).vDirection();

		return retRay;
	}

protected:
	point m_ptOrigin;
	vector m_vDirection;
};

#endif // !RAY_H_
