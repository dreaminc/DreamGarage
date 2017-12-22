#ifndef OGL_COMPOSITE_H_
#define OGL_COMPOSITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLComposite.h
// OpenGL Composite Object

#include "OGLObj.h"
#include "Primitives/composite.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLComposite : public composite, public OGLObj {
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
#pragma warning(pop)

#endif // ! OGL_COMPOSITE_H_
