#include "OGLHand.h"

OGLHand::OGLHand(OpenGLImp *pParentImp, HAND_TYPE type) :
	hand(pParentImp, type),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}

OGLHand::OGLHand(OpenGLImp *pParentImp, HAND_TYPE type, long avatarID) :
	hand(pParentImp, type, avatarID),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}