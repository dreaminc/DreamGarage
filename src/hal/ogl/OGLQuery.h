#ifndef OGL_QUERY_H_
#define OGL_QUERY_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLQuery.h

// OpenGL Query Object

#include "OGLCommon.h"
#include "OGLAttachment.h"

#include "core/types/DObject.h"

#include "core/primitives/Framebuffer.h"

class OGLImp;

#define NUM_OGL_DRAW_BUFFERS 1

class OGLQuery : public DObject {
public:
	OGLQuery(OGLImp *pParentImp);
	~OGLQuery();

	RESULT OGLInitialize();

	RESULT BeginQuery(GLenum target);
	RESULT EndQuery(GLenum target);

	RESULT GetQueryObject(GLuint *pValue);

private:
	OGLImp *m_pParentImp = nullptr;
	GLuint m_queryID;
	bool m_fPendingQuery = false;
};

#endif // ! OGL_QUERY_H_