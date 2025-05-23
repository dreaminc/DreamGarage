#ifndef OGL_CYLINDER_H_
#define OGL_CYLINDER_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLCylinder.h

// OpenGL Sphere Object

#include "OGLObj.h"

#include "core/primitives/cylinder.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLCylinder : public cylinder, public OGLObj {
public:
	OGLCylinder(OGLImp *pParentImp, double radius = 1.0f, double height = 1.0f, int numAngularDivisions = MIN_CYLINDER_DIVISIONS, int numVerticalDivisions = MIN_CYLINDER_DIVISIONS);

public:
	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	virtual RESULT Render() override;
};
#pragma warning(pop)

#endif // ! OGL_CYLINDER_H_
