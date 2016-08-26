#ifndef LINE_H_
#define LINE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/line.h
// Line primitive class

#include "point.h"
#include "vector.h"

class line {
public:
	line() :
		m_ptA(),
		m_ptB()
	{
		// Empty
	}

	line(point ptA, point ptB) :
		m_ptA(ptA),
		m_ptB(ptB)
	{
		// Empty
	}

	friend class BoundingVolume;

	// Distance of point from plane
	point_precision Distance(point ptP) {
		vector ptAToPoint = ptP - m_ptA;
		vector ptBToPoint = ptP - m_ptB;

		vector_precision planeMagnitude = GetVector().magnitude();
		vector_precision pointMagnitude = (ptAToPoint.cross(ptBToPoint)).magnitude();

		return ((pointMagnitude) / (planeMagnitude));
	}

	point ProjectedPoint(point ptP) {
		vector vAP = ptP - m_ptA;
		vector vAB = m_ptB - m_ptA;

		point ptResult = m_ptA + ((vAP.dot(vAB)) / (vAB.dot(vAB))) * vAB;

		return ptResult;
	}

	vector GetVector() {
		return (m_ptB - m_ptA);
	}

	inline point &a() { return m_ptA; }
	inline point &b() { return m_ptB; }

protected:
	point m_ptA;
	point m_ptB;
};

#endif // !BOUNDING_VOLUME_H_
