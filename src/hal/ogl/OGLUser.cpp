#include "OGLUser.h"

OGLUser::OGLUser(OGLImp *pParentImp) :
	dosuser(pParentImp),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}