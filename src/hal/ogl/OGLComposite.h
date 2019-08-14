#ifndef OGL_COMPOSITE_H_
#define OGL_COMPOSITE_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLComposite.h

// OpenGL Composite Object

#include "OGLObj.h"

#include "core/primitives/composite.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLComposite : public composite, public OGLObj {
public:
	OGLComposite(OGLImp *pParentImp);

public:
	// TODO: Need to make this better
	virtual RESULT UpdateBuffers() override;

	virtual RESULT OGLInitialize() override;
};
#pragma warning(pop)

#endif // ! OGL_COMPOSITE_H_
