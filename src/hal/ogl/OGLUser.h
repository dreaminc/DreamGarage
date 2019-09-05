#ifndef OGL_USER_H_
#define OGL_USER_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLUser.h

// OpenGL User Object
// TODO: Find a way to not have to do this for every kind of primitive and push off
// OGLComposite instead

#include "OGLObj.h"

#include "core/user/user.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLUser : public user, public OGLObj {
public:
	OGLUser(OGLImp *pParentImp);
};
#pragma warning(pop)

#endif // ! OGL_USER_H_