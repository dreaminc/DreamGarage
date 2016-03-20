#ifndef OGL_MESH_H_
#define OGL_MESH_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLMesh.h

#include "OGLObj.h"
#include "Primitives/mesh.h"

class OGLMesh : public mesh, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:

	OGLMesh(OpenGLImp *pParentImp, const std::vector<vertex>& vertices) :
		mesh(vertices),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}
};

#endif // ! OGL_MESH_H_
