#ifndef OGL_MESH_H_
#define OGL_MESH_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLModel.h

#include "OGLObj.h"
#include "Primitives/model/mesh.h"

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

	OGLMesh(OpenGLImp *pParentImp, const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) :
		mesh(vertices, indices),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	/*
	OGLMesh(OpenGLImp *pParentImp, wchar_t *pszModelName) :
		mesh(pszModelName),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}
	*/
	
	/* For DEBUG
	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	RESULT Render() override {
		RESULT r = R_PASS;

		// TODO: Rethink this since it's in the critical path
		DimObj *pDimObj = GetDimObj();

		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
		CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

		glDrawElements(GL_TRIANGLES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);
		//glDrawElements(GL_LINES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);
		//glDrawElements(GL_POINTS, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);

	Error:
		return r;
	}
	*/
};

#endif // ! OGL_MESH_H_
