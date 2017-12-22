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

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLHand : public hand, public OGLObj {
public:
	OGLHand(OpenGLImp *pParentImp, HAND_TYPE type) :
		hand(pParentImp, type),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}
};
#pragma warning(pop)

#endif // ! OGL_HAND_H_
