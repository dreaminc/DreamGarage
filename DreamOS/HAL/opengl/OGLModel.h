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

	OGLModel(OpenGLImp *pParentImp, const std::vector<vertex>& vertices) :
		model(vertices),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLModel(OpenGLImp *pParentImp, wchar_t *pszModelName) :
		model(pszModelName),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}
};

#endif // ! OGL_MODEL_H_
