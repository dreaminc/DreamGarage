#ifndef OGL_MODEL_H_
#define OGL_MODEL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLModel.h
// OpenGL Model Object
// TODO: This is the same as composite

#include "OGLObj.h"
#include "Primitives/model/model.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLModel : public model, public OGLObj {
public:
	OGLModel(OpenGLImp *pParentImp);

public:
	// TODO: Need to make this better
	RESULT UpdateBuffers() override;
};
#pragma warning(pop)

#endif // ! OGL_MODEL_H_
