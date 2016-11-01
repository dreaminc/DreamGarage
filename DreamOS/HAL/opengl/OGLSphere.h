#ifndef OGL_SPHERE_H_
#define OGL_SPHERE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLSphere.h
// OpenGL Sphere Object

#include "OGLObj.h"
#include "Primitives/sphere.h"

class OGLSphere : public sphere, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLSphere(OpenGLImp *pParentImp, float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE)) :
		sphere(radius, numAngularDivisions, numVerticalDivisions, c),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLSphere(OpenGLImp *pParentImp, BoundingSphere* pBoundingSphere, bool fTriangleBased) :
		sphere(pBoundingSphere, fTriangleBased),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	// This needs to be called from the sub-class constructor
	// or externally from the object (TODO: factory class needed)
	// TODO: Move this back to OGLObj
	RESULT OGLInitialize() {
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
		///*
		CR(m_pParentImp->glGenBuffers(1, &m_hIBO));
		CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

		// TODO: Remove convenience vars 
		dimindex *pIndex = pDimObj->IndexData();
		int pIndex_s = pDimObj->IndexDataSize();
		CR(m_pParentImp->glBufferData(GL_ELEMENT_ARRAY_BUFFER, pIndex_s, pIndex, GL_STATIC_DRAW));
		//*/

		// Enable the vertex attribute arrays
		// TODO: This needs to come out of the Implementation shader compilation, should not be static

		// Bind Position
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
		//CR(m_pParentImp->glEnableVertexAtrribArray(0));		// TEMP: Position
		//CR(m_pParentImp->EnableVertexPositionAttribute());		// TODO: Investigate performance impact of this
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)0));
		CR(m_pParentImp->glVertexAttribPointer((GLuint)0, vertex::GetPointDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetVertexOffset()));

		// Color
		//CR(m_pParentImp->glEnableVertexAtrribArray(1));		// TEMP: Color
		//CR(m_pParentImp->EnableVertexColorAttribute());		// TEMP: Position
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)1));
		CR(m_pParentImp->glVertexAttribPointer((GLuint)1, vertex::GetColorDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetColorOffset()));

		// Normal
		//CR(m_pParentImp->EnableVertexNormalAttribute());
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)2));
		CR(m_pParentImp->glVertexAttribPointer((GLuint)2, vertex::GetNormalDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetNormalOffset()));

		// UV Coordinate
		//CR(m_pParentImp->EnableVertexUVCoordAttribute());
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)3));
		CR(m_pParentImp->glVertexAttribPointer((GLuint)3, vertex::GetUVCoordDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetUVOffset()));

		// Tangent 
		//CR(m_pParentImp->EnableVertexTangentAttribute());
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)4));
		CR(m_pParentImp->glVertexAttribPointer((GLuint)4, vertex::GetTangentDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetTangentOffset()));

		// Bi-Tangent 
		//CR(m_pParentImp->EnableVertexBitangentAttribute());
		WCR(m_pParentImp->glEnableVertexAtrribArray((GLuint)5));
		CR(m_pParentImp->glVertexAttribPointer((GLuint)5, vertex::GetBitangentDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetBitangentOffset()));

		//CR(m_pParentImp->ReleaseCurrentContext());

	Error:
		return r;
	}

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	virtual RESULT Render() override {
		RESULT r = R_PASS;

		// TODO: Rethink this since it's in the critical path
		DimObj *pDimObj = GetDimObj();

		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
		CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

		GLint previousPolygonMode;
		glGetIntegerv(GL_POLYGON_MODE, &previousPolygonMode);

		if (pDimObj->IsWireframe()) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		
		// Strips
		// TODO: quad strip?
		int indexCount = 0;
		int numTriangleStripVerts = 2 * (m_numAngularDivisions + 1);
		int numStrips = m_numVerticalDivisions - 1;

		for (int i = 0; i < numStrips; i++) {
			void *pOffset = (void*)(sizeof(dimindex) * indexCount);
			glDrawElements(GL_TRIANGLE_STRIP, numTriangleStripVerts, GL_UNSIGNED_INT, pOffset);
			indexCount += numTriangleStripVerts;
		}
		
		if (pDimObj->IsWireframe()) {
			glPolygonMode(GL_FRONT_AND_BACK, previousPolygonMode);
		}


	Error:
		return r;
	}
};

#endif // ! OGL_SPHERE_H_
