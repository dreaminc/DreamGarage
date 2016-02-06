#ifndef OGL_OBJ_H_
#define OGL_OBJ_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLOBJ.h
// OpenGL Base Type - This is for coupling with the open GL implementation 

#include "OpenGLImp.h"

#define NUM_VBO 2

class OGLObj {
public:
	OGLObj(OpenGLImp *pParentImp) :
		m_pParentImp(pParentImp)
	{
		/* empty stub */
	}

	~OGLObj() {
		/* empty stub */
	}

	virtual inline vertex *VertexData() = 0;
	virtual inline int VertexDataSize() = 0;
	virtual RESULT Render() = 0;


	// This needs to be called from the sub-class constructor
	// or externally from the object (TODO: factory class needed)
	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		// Set up the Vertex Array Object (VAO)
		CR(m_pParentImp->glGenVertexArrays(1, &m_hVAO));
		CR(m_pParentImp->glBindVertexArray(m_hVAO));

		// Create Buffer Objects
		//CR(m_pParentImp->glGenBuffers(NUM_VBO, &m_hVBOs[0]));
		CR(m_pParentImp->glGenBuffers(1, &m_hVBO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));

		vertex *pVertex = VertexData();
		GLsizeiptr pVertex_n = VertexDataSize();
		CR(m_pParentImp->glBufferData(GL_ARRAY_BUFFER, pVertex_n, &pVertex[0], GL_STATIC_DRAW));
	
		/* Index Element Buffer
		//index buffer object -> we hold the index of vertex
		glGenBuffers(1, &gl_index_buffer_object);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_index_buffer_object);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		*/

		// Enable the vertex attribute arrays
		// TODO: This needs to come out of the Implementation shader compilation, should not be static

		// Bind Position
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
		CR(m_pParentImp->glEnableVertexAtrribArray(0));		// TEMP: Position
		CR(m_pParentImp->glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), vertex::GetVertexOffset()));

		// Color
		CR(m_pParentImp->glEnableVertexAtrribArray(1));		// TEMP: Color
		CR(m_pParentImp->glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), vertex::GetColorOffset()));

	Error:
		return r;
	}

	RESULT UpdateOGLBuffers() {
		RESULT r = R_PASS;

		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));

		vertex *pVertex = VertexData();
		GLsizeiptr pVertex_n = VertexDataSize();
		CR(m_pParentImp->glBufferData(GL_ARRAY_BUFFER, pVertex_n, &pVertex[0], GL_STATIC_DRAW));

	Error:
		return r;
	}

protected:
	GLuint m_hVAO;
	//GLuint m_hVBOs[NUM_VBO];
	GLuint m_hVBO;
	OpenGLImp *m_pParentImp;
};

#endif // ! OGL_OBJ_H_
