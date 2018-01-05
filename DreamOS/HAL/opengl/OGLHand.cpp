#include "OGLHand.h"

OGLHand::OGLHand(OpenGLImp *pParentImp, HAND_TYPE type) :
	hand(pParentImp, type),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}