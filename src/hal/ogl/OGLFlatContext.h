#ifndef OGL_FLAT_CONTEXT_H_
#define OGL_FLAT_CONTEXT_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLFlatContext.h

// OpenGL Flat Context Object - in the future we will likely want to 
// converge this with composite

#include "OGLObj.h"

#include "core/primitives/FlatContext.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLFlatContext : public FlatContext, public OGLObj {
public:
	OGLFlatContext(OGLImp *pParentImp);
	~OGLFlatContext();

public:
	// TODO: Need to make this better
	virtual RESULT UpdateBuffers() override;

	virtual RESULT OGLInitialize() override;
};
#pragma warning(pop)

#endif // ! OGL_FLAT_CONTEXT_H_
