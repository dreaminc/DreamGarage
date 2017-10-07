#ifndef OGL_HAND_H_
#define OGL_HAND_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLHand.h
// OpenGL Hand Object
// TODO: Find a way to not have to do this for every kind of primitive and push off
// OGLComposite instead

#include "OGLObj.h"
#include "Primitives/hand.h"

class OGLHand : public hand, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLHand(OpenGLImp *pParentImp, HAND_TYPE type) :
		hand(pParentImp, type),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}
};

#endif // ! OGL_HAND_H_
