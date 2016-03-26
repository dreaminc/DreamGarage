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
	OGLSphere(OpenGLImp *pParentImp, float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) :
		sphere(1.0f, numAngularDivisions, numVerticalDivisions),
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
		CR(m_pParentImp->EnableVertexPositionAttribute());		// TODO: Investigate performance impact of this
		CR(m_pParentImp->glVertexAttribPointer((GLuint)0, vertex::GetPointDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetVertexOffset()));

		// Color
		//CR(m_pParentImp->glEnableVertexAtrribArray(1));		// TEMP: Color
		CR(m_pParentImp->EnableVertexColorAttribute());		// TEMP: Position
		CR(m_pParentImp->glVertexAttribPointer((GLuint)1, vertex::GetColorDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetColorOffset()));

		// Normal
		CR(m_pParentImp->EnableVertexNormalAttribute());
		CR(m_pParentImp->glVertexAttribPointer((GLuint)2, vertex::GetNormalDimensions(), GetOGLPrecision(), GL_FALSE, sizeof(vertex), vertex::GetNormalOffset()));

		// TODO: UV Coord

		CR(m_pParentImp->ReleaseCurrentContext());

	Error:
		return r;
	}

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	RESULT Render() {
		RESULT r = R_PASS;

		// TODO: Rethink this since it's in the critical path
		DimObj *pDimObj = GetDimObj();

		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
		CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

		//glDrawElements(GL_TRIANGLES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);
		//glDrawElements(GL_TRIANGLE_FAN, 1 + m_numAngularDivisions, GL_UNSIGNED_INT, NULL);
		//glDrawElements(GL_POINT, pDimObj->NumberVertices(), GL_UNSIGNED_INT, NULL);

		int numVerts = pDimObj->NumberVertices();
		//int numFanVerts = m_numAngularDivisions + 1;

		//glDrawArrays(GL_TRIANGLE_FAN, 0, numFanVerts);

		//glDrawArrays(GL_POINTS, 0, numVerts);
		//glDrawArrays(GL_LINES, 0, numVerts);

		// Top Fan
		int indexCount = 0;
		int numFanVerts = m_numAngularDivisions + 2;
		glDrawElements(GL_TRIANGLE_FAN, 1 + m_numAngularDivisions + 1, GL_UNSIGNED_INT, NULL);

		// Strips
		int numTriangleStripVerts = 2 * (m_numAngularDivisions + 1);
		int numStrips = m_numVerticalDivisions - 3;
		glDrawElements(GL_TRIANGLE_FAN, 1 + m_numAngularDivisions + 1, GL_UNSIGNED_INT, sizeof(dimindex) * );
		

		// TODO: Bottom Fan


	Error:
		return r;
	}
};

#endif // ! OGL_SPHERE_H_
