#include "Simplex.h"

#include "Primitives/vector.h"

Simplex::Simplex() :
	m_points_n(0)
{
	// empty
}

RESULT Simplex::ClearPoints() {
	//memset(m_points, 0, sizeof(m_points));
	m_points_n = 0;

	m_ptA = point();
	m_ptB = point();
	m_ptC = point();
	m_ptD = point();

	return R_PASS;
}

RESULT Simplex::AddPoint(point ptNewPoint) {
	RESULT r = R_PASS;

	m_ptD = m_ptC;
	m_ptC = m_ptB;
	m_ptB = m_ptA;
	m_ptA = ptNewPoint;

	if(m_points_n != MAX_SIMPLEX_POINTS)
		m_points_n++;

//Error:
	return r;
}

// TODO: Split into two functions
bool Simplex::CheckSimplex(vector *pvDirection) {

	// This shouldn't occur
	if (m_points_n == 0)
		return false;

	// Direction to the origin from new point
	vector vAO = point(0.0f, 0.0f, 0.0f) - m_ptA;

	switch (m_points_n) {

		// Point
		case 1: {
			// [] = [A] v = vAO
			*pvDirection = vAO;
			pvDirection->Normalize();
		} break;

			// Line
		case 2: {
			vector vAB = m_ptB - m_ptA;

			if (vAB.dot(vAO) >= 0.0f) {
				// [] = [A, B] v = vAB x vAO x vAB
				m_points_n = 2;
				*pvDirection = (vAB.cross(vAO)).cross(vAB);
				pvDirection->Normalize();
			}
			else {
				// [] = [A] v = vAO
				// This removes point B from simplex 
				// since point A is closer to origin on line
				m_points_n = 1;
				*pvDirection = vAO;
				pvDirection->Normalize();
			}
		} break;

			// Triangle
		case 3: {
			vector vAB = (vector)(m_ptB - m_ptA);
			vector vAC = (vector)(m_ptC - m_ptA);

			vector vABC = vAB.cross(vAC);

			if ((vABC.cross(vAC)).dot(vAO) >= 0.0f) {
				if (vAC.dot(vAO) > 0.0f) {
					// [] = [A, C] v = vAC x vAO x vAC
					m_points_n = 2;
					m_ptB = m_ptC;
					*pvDirection = (vAC.cross(vAO)).cross(vAC);
					pvDirection->Normalize();
				}
				// CASE X duplicated below
				else if (vAB.dot(vAO) >= 0.0f) {
					// [] = [A, B] v = vAB x vAO x vAB
					m_points_n = 2;
					*pvDirection = (vAB.cross(vAO)).cross(vAB);
					pvDirection->Normalize();
				}
				else {
					// [] = [A] c = vAO
					m_points_n = 1;
					*pvDirection = vAO;
					pvDirection->Normalize();
				}
			}
			else {
				if ((vAB.cross(vABC)).dot(vAO) >= 0.0f) {

					// CASE X duplicated from above
					if (vAB.dot(vAO) >= 0.0f) {
						// [] = [A, B] v = vAB x vAO x vAB
						m_points_n = 2;
						*pvDirection = (vAB.cross(vAO)).cross(vAB);
						pvDirection->Normalize();
					}
					else {
						// [] = [A] v = vAO
						m_points_n = 1;
						*pvDirection = vAO;
						pvDirection->Normalize();
					}
				}
				else {
					// Above or below triangle
					if (vABC.dot(vAO) >= 0.0f) {
						// [] = [A, B, C] v = vABC
						m_points_n = 3;
						*pvDirection = vABC;
						pvDirection->Normalize();
					}
					else {
						m_points_n = 3;

						// [] = [A, C, B] v = -vABC
						point ptTemp = m_ptB;
						m_ptB = m_ptC;
						m_ptC = ptTemp;

						*pvDirection = -1.0f * vABC;
						pvDirection->Normalize();
					}
				}
			}

		} break;

		// Tetrahedron
		case 4: {
			vector vAB = (vector)(m_ptB - m_ptA);
			vector vAC = (vector)(m_ptC - m_ptA);
			vector vAD = (vector)(m_ptD - m_ptA);

			vector vABC = vAB.cross(vAC);
			vector vACD = vAC.cross(vAD);
			vector vADB = vAD.cross(vAB);

			if (vABC.dot(vAO) > 0.0f) {
				// [] = [A, B, C] v = vABC
				m_points_n = 3;

				//*pvDirection = vABC;
				//pvDirection->Normalize();

				// Call the triangle case
				return CheckSimplex(pvDirection);
			}
			else if (vACD.dot(vAO) > 0.0f) {
				// [] = [A, C, D] v = vACD

				m_points_n = 3;
				m_ptB = m_ptC;
				m_ptC = m_ptD;

				//*pvDirection = vACD;
				//pvDirection->Normalize();

				// Call the triangle case
				return CheckSimplex(pvDirection);
			}
			else if (vADB.dot(vAO) > 0.0f) {
				// [] = [A, B, D] v = vADB
				m_points_n = 3;
				m_ptC = m_ptD;

				//*pvDirection = vADB;
				//pvDirection->Normalize();

				// Call the triangle case
				return CheckSimplex(pvDirection);
			}
			else {
				// We've enclosed the origin
				return true;
			}

		} break;
	}

	return false;
}

bool Simplex::UpdateSimplex(point ptNewSupport, vector *pvDirection) {
	if (ptNewSupport.IsZero())
		return true;
	
	AddPoint(ptNewSupport);

	return CheckSimplex(pvDirection);
}