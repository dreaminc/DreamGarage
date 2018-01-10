#include "line.h"

line::line() :
	m_ptA(),
	m_ptB()
{
	// Empty
}

line::line(point ptA, point ptB) :
	m_ptA(ptA),
	m_ptB(ptB)
{
	// Empty
}

// Distance of point from plane
point_precision line::Distance(point ptP) {
	vector ptAToPoint = ptP - m_ptA;
	vector ptBToPoint = ptP - m_ptB;

	vector_precision planeMagnitude = GetVector().magnitude();
	vector_precision pointMagnitude = (ptAToPoint.cross(ptBToPoint)).magnitude();

	return ((pointMagnitude) / (planeMagnitude));
}

//point_precision line::Distance(line l) {
point_precision line::Distance(line l, point *pptClosestLineA, point *pptClosestLineB, vector *pvNormal) {
	vector vCross = GetVector().cross(l.GetVector());
	
	point_precision retVal = vCross.dot((m_ptA - l.m_ptA));
	retVal /= vCross.magnitude();

	if (pvNormal != nullptr) {
		*pvNormal = GetVector().cross(l.GetVector()).Normal();

		// Find the closest respective points
		// https://en.wikipedia.org/wiki/Skew_lines#Distance_between_two_skew_lines
		
		vector vNormalA = GetVector().cross(*pvNormal);
		vector vNormalB = l.GetVector().cross(*pvNormal);

		if (pptClosestLineA != nullptr) {
			
			double dotProdDiffNormalA = (l.a() - a()).dot(vNormalB);
			double dotProdNormalA = GetVector().dot(vNormalB);
			double tA = dotProdDiffNormalA / dotProdNormalA;
			
			*pptClosestLineA = a() + GetVector() * tA;
		}

		if (pptClosestLineB != nullptr) {
			double dotProdDiffNormalB = (a() - l.a()).dot(vNormalA);
			double dotProdNormalB = l.GetVector().dot(vNormalA);
			double tB = dotProdDiffNormalB / dotProdNormalB;

			*pptClosestLineB = l.a() + l.GetVector() * tB;
		}
	}

	return retVal;
}

point line::ProjectedPoint(point ptP) {
	vector vAP = ptP - m_ptA;
	vector vAB = m_ptB - m_ptA;

	point ptResult = m_ptA + (vAB * ((vAP.dot(vAB)) / (vAB.dot(vAB))));

	return ptResult;
}

vector line::GetVector() {
	vector v = (m_ptB - m_ptA);
	//return v.Normal();
	return v;
}

ray line::GetRay() {
	return ray(m_ptA, (m_ptB - m_ptA).Normal());
}

float line::length() {
	return (float)((m_ptB - m_ptA).magnitude());
}

RESULT line::Translate(vector vTranslate) {
	m_ptA += vTranslate;
	m_ptB += vTranslate;

	return R_SUCCESS;
}

RESULT line::ApplyMatrix(matrix<point_precision, 4, 4> mat) {
	m_ptA = mat * m_ptA;
	m_ptB = mat * m_ptB;

	return R_SUCCESS;
}
