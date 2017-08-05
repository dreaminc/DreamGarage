#ifndef OGL_RENDERBUFFER_H_
#define OGL_RENDERBUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/HAL/opengl/OGLRenderbuffer.h
// OpenGL Renderbuffer Primitive

#include "HAL/opengl/OpenGLImp.h"

class OGLRenderbuffer {
public:
	OGLRenderbuffer(OpenGLImp *pParentImp, int width, int height, int sampleCount);
	~OGLRenderbuffer();

	RESULT OGLInitialize();
	RESULT OGLDelete();
	
	GLuint GetOGLRenderbufferIndex();

	RESULT Resize(int pxWidth, int pxHeight);

private:
	OpenGLImp *m_pParentImp;

	GLuint m_OGLRenderbufferIndex;

	int m_width;
	int m_height;
	int m_sampleCount;
};

#endif // ! OGL_RENDERBUFFER_H_