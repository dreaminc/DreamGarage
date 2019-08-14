#ifndef OGL_MESH_H_
#define OGL_MESH_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLModel.h

#include "OGLObj.h"

#include "core/model/mesh.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLMesh : public mesh, public OGLObj {
public:
	OGLMesh(OGLImp *pParentImp, mesh::params *pMeshParams);
	OGLMesh(OGLImp *pParentImp, const std::vector<vertex>& vertices);
	OGLMesh(OGLImp *pParentImp, const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	/* For DEBUG
	OGLMesh(OGLImp *pParentImp, wchar_t *pszModelName);

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	RESULT Render() override;
	//*/
};
#pragma warning(pop)

#endif // ! OGL_MESH_H_
