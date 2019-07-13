#ifndef OGL_PLANE_H_
#define OGL_PLANE_H_

#include "RESULT/EHM.h"

#include "OGLObj.h"
#include "Primitives/DimPlane.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLPlane : public DimPlane, public OGLObj {
public:
	OGLPlane(OpenGLImp *pParentImp, point ptOrigin = point(), vector vNormal = vector::jVector(1.0f));
	virtual RESULT Render() override;
};
#pragma warning(pop)


#endif // ! OGL_PLANE_H_