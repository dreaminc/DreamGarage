#ifndef OGL_RAY_H_
#define OGL_RAY_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLRay.h

#include "OGLObj.h"

#include "core/dimension/DimRay.h"

#include "core/primitives/point.h"
#include "core/primitives/vector.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLRay : public DimRay, public OGLObj {
public:
	OGLRay(OGLImp *pParentImp, point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);
	virtual RESULT Render() override;
};
#pragma warning(pop)


#endif // ! OGL_RAY_H_