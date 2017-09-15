#ifndef OGL_MODEL_H_
#define OGL_MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLModel.h
// OpenGL Model Object
// TODO: This is the same as composite

#include "OGLObj.h"
#include "Primitives/model/model.h"

class OGLModel : public model, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLModel(OpenGLImp *pParentImp) :
		model(pParentImp),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	// TODO: Need to make this better
	RESULT UpdateBuffers() override {
		return UpdateOGLBuffers();
	}
};

#endif // ! OGL_MODEL_H_
