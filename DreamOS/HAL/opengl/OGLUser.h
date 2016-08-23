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

class OGLUser : public user, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLUser(OpenGLImp *pParentImp) :
		user(pParentImp),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}
};

#endif // ! OGL_USER_H_