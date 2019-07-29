#ifndef CAPTURED_OBJ_H_
#define CAPTURED_OBJ_H_

// Dream Interaction Engine Module
// dos/src/module/InteractionEngine/CapturedObj.h

// Captured Object for Interaction Engine

#include "core/primitives/plane.h"

class VirtualObj;
class point;
class vector;

class CapturedObj {
public:
	CapturedObj(VirtualObj *pObj, float threshold, plane planeContext, point ptOffset, point ptOrigin, vector vSurface);
	~CapturedObj();

	VirtualObj *GetObject();
	float GetThreshold();
	plane GetPlaneContext();
	point GetOffset();
	point GetOrigin();
	vector GetRelativeSurfaceNormal();

private:
	VirtualObj *m_pObj;
	float m_threshold;
	plane m_planeContext;
	point m_ptOffset;
	point m_ptOrigin;
	vector m_vRelativeSurfaceNormal;
};
	

#endif // ! CAPTURED_OBJ_H_