#ifndef OGL_VOLUME_H_
#define OGL_VOLUME_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLVolume.h

// OpenGL Volume Object

#include "OGLObj.h"

#include "core/primitives/volume.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLVolume : public volume, public OGLObj {
public:
	OGLVolume(OGLImp *pParentImp);
	OGLVolume(OGLImp *pParentImp, volume::params *pVolumeParams);
	OGLVolume(OGLImp *pParentImp, double width, double length, double height, bool fTriangleBased);
	OGLVolume(OGLImp *pParentImp, double side, bool fTriangleBased);
	OGLVolume(OGLImp *pParentImp, BoundingBox* pBoundingBox, bool fTriangleBased);

public:
	RESULT UpdateFromBoundingBox(BoundingBox* pBoundingBox);
	RESULT UpdateFromVertices(DimObj *pDimObj);

	// TODO: Need to make this better
	virtual RESULT UpdateBuffers() override;

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	// Critical path, EHM removed
	// manually debug
	virtual RESULT Render() override;
};

#pragma warning(pop)

#endif // ! OGL_QUAD_H_
