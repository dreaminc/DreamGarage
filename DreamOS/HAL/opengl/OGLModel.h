#ifndef OGL_MODEL_H_
#define OGL_MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLModel.h

#include "OGLObj.h"
#include "Primitives/model.h"

class OGLModel : public model, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLModel(OpenGLImp *pParentImp, model *pModel) :
		model(pModel),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		// RESULT r = OGLInitialize();
	}

	RESULT LoadFile(std::ifstream *pFile) {
		return R_NOT_IMPLEMENTED;
	}
};

#endif // ! OGL_MODEL_H_
