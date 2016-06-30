#ifndef OGL_OBJ_H_
#define OGL_OBJ_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLOBJ.h
// OpenGL Base Type - This is for coupling with the open GL implementation 

#include "OpenGLImp.h"
#include "Primitives/DimObj.h"

#define NUM_VBO 2

class OGLObj {
public:
	OGLObj(OpenGLImp *pParentImp) :
		m_pParentImp(pParentImp)
	{
		/* empty stub */
	}

	~OGLObj() {
		ReleaseOGLBuffers();
	}

	//virtual inline vertex *VertexData() = 0;
	//virtual inline int VertexDataSize() = 0;

	//virtual RESULT Render() = 0;
	virtual DimObj *GetDimObj() = 0;

	RESULT ReleaseOGLBuffers() {
		RESULT r = R_PASS;

		if (m_pParentImp != NULL) {
			if (m_hVBO != NULL) {
				CR(m_pParentImp->glDeleteBuffers(1, &m_hVBO));
				m_hVBO = NULL;
			}

			if (m_hIBO != NULL) {
				CR(m_pParentImp->glDeleteBuffers(1, &m_hIBO));
				m_hIBO = NULL;
			}

			if (m_hVAO != NULL) {
				CR(m_pParentImp->glDeleteVertexArrays(1, &m_hVAO));
				m_hVAO = NULL;
			}
		}

	Error:
		return r;
	}

	// This should be used in the OGLInitialize function
	inline GLushort GetOGLPrecision() {
		#ifdef FLOAT_PRECISION
			return GL_FLOAT;
		#elif defined(DOUBLE_PRECISION)
			return GL_DOUBLE;
		#endif
	}

	// This needs to be called from the sub-class constructor
	// or externally from the object (TODO: factory class needed)
	virtual RESULT OGLInitialize() {
		RESULT r = R_PASS;

		DimObj *pDimObj = GetDimObj();
		CR(m_pParentImp->MakeCurrentContext());

		// Set up the Vertex Array Object (VAO)
		CR(m_pParentImp->glGenVertexArrays(1, &m_hVAO));
		CR(m_pParentImp->glBindVertexArray(m_hVAO));

		// Create Buffer Objects
		//CR(m_pParentImp->glGenBuffers(NUM_VBO, &m_hVBOs[0]));
		CR(m_pParentImp->glGenBuffers(1, &m_hVBO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));

		// TODO: Remove convenience vars 
		vertex *pVertex = pDimObj->VertexData();
		GLsizeiptr pVertex_n = pDimObj->VertexDataSize();
		CR(m_pParentImp->glBufferData(GL_ARRAY_BUFFER, pVertex_n, &pVertex[0], GL_STATIC_DRAW));
	
		// Index Element Buffer
		CR(m_pParentImp->glGenBuffers(1, &m_hIBO));
		CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

		// TODO: Remove convenience vars 
		dimindex *pIndex = pDimObj->IndexData();
		int pIndex_s = pDimObj->IndexDataSize();
		CR(m_pParentImp->glBufferData(GL_ELEMENT_ARRAY_BUFFER, pIndex_s, pIndex, GL_STATIC_DRAW));

		// Enable the vertex attribute arrays
		// TODO: This needs to come out of the Implementation shader compilation, should not be static
		// TODO: Might want to encapsulate this in a VertexDecleration / VertexAttributeDeclaration or simply
		// have it fall out of the actual shader

		// Bind Position
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
		//CR(m_pParentImp->EnableVertexPositionAttribute());		// TODO: Investigate performance impact of this
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)0));
		CR(m_pParentImp->glVertexAttribPointer((GLuint)0, vertex::GetPointDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetVertexOffset()));

		// Color
		//WCR(m_pParentImp->EnableVertexColorAttribute());		// TEMP: Position
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)1));
		WCR(m_pParentImp->glVertexAttribPointer((GLuint)1, vertex::GetColorDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetColorOffset()));

		// Normal
		//WCR(m_pParentImp->EnableVertexNormalAttribute());	
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)2));
		WCR(m_pParentImp->glVertexAttribPointer((GLuint)2, vertex::GetNormalDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetNormalOffset()));

		// UV Coordinate
		//WCR(m_pParentImp->EnableVertexUVCoordAttribute());
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)3));
		WCR(m_pParentImp->glVertexAttribPointer((GLuint)3, vertex::GetUVCoordDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetUVOffset()));

		// Tangent 
		//WCR(m_pParentImp->EnableVertexTangentAttribute());
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)4));
		WCR(m_pParentImp->glVertexAttribPointer((GLuint)4, vertex::GetTangentDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetTangentOffset()));

		// Bi-Tangent 
		//WCR(m_pParentImp->EnableVertexBitangentAttribute());
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)5));
		WCR(m_pParentImp->glVertexAttribPointer((GLuint)5, vertex::GetBitangentDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetBitangentOffset()));

		//CR(m_pParentImp->ReleaseCurrentContext());

	Error:
		return r;
	}

	RESULT UpdateOGLBuffers() {
		RESULT r = R_PASS;

		DimObj *pDimObj = GetDimObj();
		CNM(pDimObj, "Failed to acquire Dimension Object");

		CR(m_pParentImp->MakeCurrentContext());
		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));

		vertex *pVertex = pDimObj->VertexData();
		GLsizeiptr pVertex_n = pDimObj->VertexDataSize();
		CR(m_pParentImp->glBufferData(GL_ARRAY_BUFFER, pVertex_n, &pVertex[0], GL_STATIC_DRAW));

		// Usually we prefer to release the context so that it won't get mistakenly used by next OGL calls.
		// We're constantly getting error 1282 from wglMakeContext(null, null).
		// For now, removing this safety.
		// CR(m_pParentImp->ReleaseCurrentContext());

	Error:
		return r;
	}

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	//virtual RESULT Render() {
	virtual RESULT Render() {
		RESULT r = R_PASS;

		// TODO: Rethink this since it's in the critical path
		DimObj *pDimObj = GetDimObj();

		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
		CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

		glDrawElements(GL_TRIANGLES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);
		//glDrawElements(GL_POINT, pDimObj->NumberVertices(), GL_UNSIGNED_INT, NULL);

	Error:
		return r;
	}

	OGLTexture *GetColorTexture() {
		DimObj *pDimObj = GetDimObj();
		OGLTexture *pTexture = reinterpret_cast<OGLTexture*>(pDimObj->GetColorTexture());
		return pTexture;
	}

	OGLTexture *GetBumpTexture() {
		DimObj *pDimObj = GetDimObj();
		OGLTexture *pTexture = reinterpret_cast<OGLTexture*>(pDimObj->GetBumpTexture());
		return pTexture;
	}

protected:
	GLuint m_hVAO;		// vertex array object
	GLuint m_hVBO;		// vertex buffer object
	GLuint m_hIBO;		// index buffer object
	OpenGLImp *m_pParentImp;
};

#endif // ! OGL_OBJ_H_
