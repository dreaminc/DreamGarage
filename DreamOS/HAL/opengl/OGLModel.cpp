#include "OGLModel.h"

OGLModel::OGLModel(OpenGLImp *pParentImp) :
	model(pParentImp),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}

// TODO: Need to make this better
RESULT OGLModel::UpdateBuffers() {
	return UpdateOGLBuffers();
}