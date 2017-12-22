#ifndef OGL_USER_H_
#define OGL_USER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLUser.h
// OpenGL User Object
// TODO: Find a way to not have to do this for every kind of primitive and push off
// OGLComposite instead

#include "OGLObj.h"
#include "Primitives/user.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLUser : public user, public OGLObj {
public:

	OGLUser(OpenGLImp *pParentImp) :
		user(pParentImp),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}
};
#pragma warning(pop)

#endif // ! OGL_USER_H_