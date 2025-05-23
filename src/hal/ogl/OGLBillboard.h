#ifndef OGL_BILLBOARD_H_
#define OGL_BILLBOARD_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLBillBoard.h

#include "OGLObj.h"

// TODO: Review
#include "core/primitives/billboard.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLBillboard : public billboard, public OGLObj {
public:
	OGLBillboard(OGLImp *pParentImp, point ptOrigin, float width, float height);
	~OGLBillboard();

	// override to specify GL_POINT
	virtual RESULT Render() override;
};
#pragma warning(pop)

#endif // ! OGL_BILLBOARD_H_