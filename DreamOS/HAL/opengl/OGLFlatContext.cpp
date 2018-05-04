#include "OGLFlatContext.h"

OGLFlatContext::OGLFlatContext(OpenGLImp *pParentImp) :
	FlatContext(pParentImp),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}

OGLFlatContext::~OGLFlatContext() {
	// empty
}

// TODO: Need to make this better
RESULT OGLFlatContext::UpdateBuffers() {
	return UpdateOGLBuffers();
}