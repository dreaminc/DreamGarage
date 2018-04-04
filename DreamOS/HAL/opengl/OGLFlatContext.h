#ifndef OGL_FLAT_CONTEXT_H_
#define OGL_FLAT_CONTEXT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLFlatContext.h
// OpenGL Flat Context Object - in the future we will likely want to 
// converge this with composite

#include "OGLObj.h"
#include "Primitives/FlatContext.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLFlatContext : public FlatContext, public OGLObj {
public:
	OGLFlatContext(OpenGLImp *pParentImp);

public:
	// TODO: Need to make this better
	virtual RESULT UpdateBuffers() override;
};
#pragma warning(pop)

#endif // ! OGL_FLAT_CONTEXT_H_
