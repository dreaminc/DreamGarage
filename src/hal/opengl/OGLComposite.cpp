#include "OGLComposite.h"

OGLComposite::OGLComposite(OpenGLImp *pParentImp) :
	composite(pParentImp),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}

// TODO: Need to make this better
RESULT OGLComposite::UpdateBuffers() {
	return UpdateOGLBuffers();
}

RESULT OGLComposite::OGLInitialize() {
	return R_SKIPPED;
}