#ifndef OGL_PLANE_H_
#define OGL_PLANE_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLPlane.h

#include "OGLObj.h"

#include "core/dimension/DimPlane.h"

#include "core/primitives/point.h"
#include "core/primitives/vector.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLPlane : public DimPlane, public OGLObj {
public:
	OGLPlane(OGLImp *pParentImp, point ptOrigin = point(), vector vNormal = vector::jVector(1.0f));
	virtual RESULT Render() override;
};
#pragma warning(pop)


#endif // ! OGL_PLANE_H_