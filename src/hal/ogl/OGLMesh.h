#ifndef OGL_MESH_H_
#define OGL_MESH_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLModel.h

#include "OGLObj.h"
#include "Primitives/model/mesh.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLMesh : public mesh, public OGLObj {
public:
	OGLMesh(OpenGLImp *pParentImp, mesh::params *pMeshParams);
	OGLMesh(OpenGLImp *pParentImp, const std::vector<vertex>& vertices);
	OGLMesh(OpenGLImp *pParentImp, const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	/* For DEBUG
	OGLMesh(OpenGLImp *pParentImp, wchar_t *pszModelName);

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	RESULT Render() override;
	//*/
};
#pragma warning(pop)

#endif // ! OGL_MESH_H_
