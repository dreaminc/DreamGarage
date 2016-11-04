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
	OGLObj(OpenGLImp *pParentImp);
	~OGLObj();

	//virtual inline vertex *VertexData() = 0;
	//virtual inline int VertexDataSize() = 0;

	//virtual RESULT Render() = 0;
	virtual DimObj *GetDimObj() = 0;

	RESULT ReleaseOGLBuffers();
	GLushort GetOGLPrecision();

	// This needs to be called from the sub-class constructor
	// or externally from the object (TODO: factory class needed)
	virtual RESULT OGLInitialize();
	RESULT UpdateOGLBuffers();

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	//virtual RESULT Render() {
	virtual RESULT Render();
	virtual RESULT RenderBoundingVolume();
	virtual RESULT UpdateBoundingVolume();

	OGLTexture *GetColorTexture();
	OGLTexture *GetBumpTexture();
	OGLTexture *GetTextureAmbient();
	OGLTexture *GetTextureDiffuse();
	OGLTexture *GetTextureSpecular();

	OGLObj *GetOGLBoundingVolume();

protected:
	GLuint m_hVAO;		// vertex array object
	GLuint m_hVBO;		// vertex buffer object
	GLuint m_hIBO;		// index buffer object

	OpenGLImp *m_pParentImp;

private:
	OGLObj *m_pOGLBoundingVolume;
};

#endif // ! OGL_OBJ_H_
