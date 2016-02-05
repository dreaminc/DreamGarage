#ifndef OGL_OBJ_H_
#define OGL_OBJ_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLOBJ.h
// OpenGL Base Type - This is for coupling with the open GL implementation 

#include "OpenGLImp.h"

class OGLObj {
public:
	OBLObj(OpenGLImp *pParentImp) :
		m_pParentImp(pParentImp)
	{
		/* empty stub */
	}

	~OGLObj() {
		/* empty stub */
	}

	virtual inline void *VertexData() = 0;
	virtual inline int VertexDataSize() = 0;
	virtual RESULT Render = 0;

	RESULT OGLInitialize() {
		RESULT r = R_PASS;
		// Create Buffer Objects
		CR(m_pParentImp->glGenBuffers(2, m_hVBOs));

		// For readability
		GLuint positionBufferHandle = m_hVBOs[0];
		GLuint colorBufferHandle = m_hVBOs[1];

		// Position Buffer
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle));
		CR(m_pParentImp->glBufferData(GL_ARRAY_BUFFER, VertexDataSize(), VertexData(), GL_STATIC_DRAW);

		// Color Buffer
		// TODO: Is this needed with a custom vertex structure? 
		//CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle));
		//CR(m_pParentImp->glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vertex), vertTemp, GL_STATIC_DRAW);

		// Set up the Vertex Array Object (VAO)
		CR(m_pParentImp->glGenVertexArrays(1, &m_hVAO));
		CR(m_pParentImp->glBindVertexArray(m_hVAO));

		// Enable the vertex attribute arrays
		// TODO: This needs to come out of the Implementation shader compilation, should not be static
		CR(m_pParentImp->glEnableVertexAtrribArray(0));		// TEMP: Position
		CR(m_pParentImp->glEnableVertexAtrribArray(1));		// TEMP: Color

		// Bind Position
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle));
		CR(m_pParentImp->glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)NULL));

		// Bind Color
		// TODO: Programatically get the address from vertex object
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle));
		CR(m_pParentImp->glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(vertex) / 2)));

	Error:
		return r;
	}

protected:
	GLuint m_hVAO;
	GLuint m_hVBOs[2];
	OpenGLImp *m_pParentImp;
};

#endif // ! OGL_OBJ_H_
