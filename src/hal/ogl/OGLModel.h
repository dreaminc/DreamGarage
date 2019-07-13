#ifndef OGL_MODEL_H_
#define OGL_MODEL_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLModel.h

// OpenGL Model Object
// TODO: This is the same as composite

#include "OGLObj.h"

#include "core/model/model.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLModel : public model, public OGLObj {
public:
	OGLModel(OGLImp *pParentImp);

public:
	// TODO: Need to make this better
	RESULT UpdateBuffers() override;
};
#pragma warning(pop)

#endif // ! OGL_MODEL_H_
