#include "CapturedObj.h"
#include "VirtualObj.h"

CapturedObj::CapturedObj(VirtualObj *pObj, float threshold, plane planeContext, point ptOffset, point ptOrigin, vector vSurface) :
	m_pObj(pObj),
	m_threshold(threshold),
	m_planeContext(planeContext),
	m_ptOffset(ptOffset),
	m_ptOrigin(ptOrigin),
	m_vRelativeSurfaceNormal(vSurface) {

}


CapturedObj::~CapturedObj() {
	// empty
}

VirtualObj *CapturedObj::GetObject() {
	return m_pObj;
}

float CapturedObj::GetThreshold() {
	return m_threshold;
}

plane CapturedObj::GetPlaneContext() {
	return m_planeContext;
}

point CapturedObj::GetOffset() {
	return m_ptOffset;
}

point CapturedObj::GetOrigin() {
	return m_ptOrigin;
}

vector CapturedObj::GetRelativeSurfaceNormal() {
	return m_vRelativeSurfaceNormal;
}
