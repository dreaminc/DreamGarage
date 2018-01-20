#include "OGLUser.h"

OGLUser::OGLUser(OpenGLImp *pParentImp) :
	user(pParentImp),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}