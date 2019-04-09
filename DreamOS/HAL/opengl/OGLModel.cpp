#include "OGLModel.h"

OGLModel::OGLModel(OpenGLImp *pParentImp) :
	model(pParentImp),
	OGLObj(pParentImp)
{
	//
}

// TODO: Need to make this better
RESULT OGLModel::UpdateBuffers() {
	return UpdateOGLBuffers();
}