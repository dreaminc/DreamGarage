#ifndef SIMPLEX_H_
#define SIMPLEX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/PhysicsEngine/Simplex.h
// An object representing a 3D simplex

#include "Primitives/point.h"

#define MAX_SIMPLEX_POINTS 4

class Simplex {
public:
	Simplex();

	RESULT AddPoint(point ptNewPoint);

	RESULT ClearPoints();
	bool UpdateSimplex(point ptNewSupport, vector *pvDirection);

private:
	bool CheckSimplex(vector *pvDirection);

private:
	//point m_points[MAX_SIMPLEX_POINTS];	// Use a static array for cache coherency 

	point m_ptA;
	point m_ptB;
	point m_ptC;
	point m_ptD;

	int m_points_n = 0;
	//int m_points_c = 0;
};

#endif	// !SIMPLEX_H_