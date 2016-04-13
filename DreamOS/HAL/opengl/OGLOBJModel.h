#ifndef OGL_OBJ_MODEL_H_
#define OGL_OBJ_MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLOBJModel.h
// This is the OBL extension of the OGL model object
// This is to avoid the need to create any kind of data duplication
// but it might make sense to instead use the OGL Object class 
// to simply pipe data between the OGL OBJ implementation 

// NOTE: THIS HAS BEEN DEPRECATED 

#include "OGLObj.h"
#include "Primitives/OBJModel.h"

class OGLOBJModel : public OBJModel, public OGLObj  {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLOBJModel(OpenGLImp *pParentImp) :
		OBJModel(),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		// RESULT r = OGLInitialize();
	}
};

#endif	// ! OGL_OBJ_MODEL_H_
