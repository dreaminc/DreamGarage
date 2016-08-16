#ifndef OGL_COMPOSITE_H_
#define OGL_COMPOSITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLComposite.h
// OpenGL Composite Object

#include "OGLObj.h"
#include "Primitives/composite.h"

class OGLComposite : public composite, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLComposite(OpenGLImp *pParentImp) :
		composite(pParentImp),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	// TODO: Need to make this better
	RESULT UpdateBuffers() override {
		return UpdateOGLBuffers();
	}
};

#endif // ! OGL_COMPOSITE_H_
