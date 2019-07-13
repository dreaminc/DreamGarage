#ifndef OGL_HAND_H_
#define OGL_HAND_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLHand.h

// OpenGL Hand Object
// TODO: Find a way to not have to do this for every kind of primitive and push off
// OGLComposite instead
// TODO: Fuck this hand class

#include "OGLObj.h"

#include "core/hand/hand.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLHand : public hand, public OGLObj {
public:
	OGLHand(OGLImp *pParentImp, HAND_TYPE type);
	OGLHand(OGLImp *pParentImp, HAND_TYPE type, long avatarID);
};
#pragma warning(pop)

#endif // ! OGL_HAND_H_
