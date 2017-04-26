#ifndef OGL_QUERY_H_
#define OGL_QUERY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/HAL/opengl/OGLQuery.h
// OpenGL Query Object

#include "OpenGLCommon.h"
#include "Primitives/Framebuffer.h"

#include "OGLDepthbuffer.h"

class OpenGLImp;

#define NUM_OGL_DRAW_BUFFERS 1

class OGLQuery {
public:
	OGLQuery(OpenGLImp *pParentImp);
	~OGLQuery();

	RESULT OGLInitialize();

	RESULT BeginQuery(GLenum target);
	RESULT EndQuery(GLenum target);

	RESULT GetQueryObject(GLuint *pValue);

private:
	OpenGLImp *m_pParentImp = nullptr;
	GLuint m_queryID;
	bool m_fPendingQuery = false;
};

#endif // ! OGL_QUERY_H_