#ifndef LINE_H_
#define LINE_H_

#include "core/ehm/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/line.h
// Line primitive class

#include "point.h"
#include "vector.h"
#include "ray.h"

class line {
	friend class BoundingVolume;

public:
	line();
	line(point ptA, point ptB);


	// Distance of point from plane
	point_precision Distance(point ptP);
	point_precision Distance(line l, point *pptClosestLineA = nullptr, point *pptClosestLineB = nullptr, vector *pvNormal = nullptr);
	point_precision SegmentDistance(line l, point *pptClosestLineA = nullptr, point *pptClosestLineB = nullptr, vector *pvNormal = nullptr);
	
	point ProjectedPoint(point ptP);

	vector GetVector();
	ray GetRay();

	float length();

	RESULT Translate(vector vTranslate);
	RESULT ApplyMatrix(matrix<point_precision, 4, 4> mat);

	inline point &a() { return m_ptA; }
	inline point &b() { return m_ptB; }

protected:
	point m_ptA;
	point m_ptB;
};

#endif // !BOUNDING_VOLUME_H_

