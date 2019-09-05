#ifndef OGL_SPHERE_H_
#define OGL_SPHERE_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLSphere.h

// OpenGL Sphere Object

#include "OGLObj.h"

#include "core/primitives/sphere.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLSphere : public sphere, public OGLObj {
public:
	OGLSphere(OGLImp *pParentImp, sphere::params *pSphereParams);
	OGLSphere(OGLImp *pParentImp, float radius = 1.0f, int numAngularDivisions = 10, int numVerticalDivisions = 10, color c = color(COLOR_WHITE));
	OGLSphere(OGLImp *pParentImp, BoundingSphere* pBoundingSphere, bool fTriangleBased);

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	virtual RESULT Render() override;

	RESULT UpdateFromBoundingSphere(BoundingSphere* pBoundingSphere);
};
#pragma warning(pop)

#endif // ! OGL_SPHERE_H_
