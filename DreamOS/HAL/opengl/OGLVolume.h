#ifndef OGL_VOLUME_H_
#define OGL_VOLUME_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLVolume.h
// OpenGL Volume Object

#include "OGLObj.h"
#include "Primitives/volume.h"

class OGLVolume : public volume, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLVolume(OpenGLImp *pParentImp) :
		volume(1.0f),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLVolume(OpenGLImp *pParentImp, double side) :
		volume(side),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}
};

#endif // ! OGL_QUAD_H_
