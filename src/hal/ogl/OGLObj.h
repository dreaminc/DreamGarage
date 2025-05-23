#ifndef OGL_OBJ_H_
#define OGL_OBJ_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLOBJ.h

// OpenGL Base Type - This is for coupling with the open GL implementation 

#include <functional>

#include "OGLImp.h"

#include "core/dimension/DimObj.h"

#define NUM_VBO 2

class OGLObj : public virtual DimObj {
public:
	OGLObj(OGLImp *pParentImp);
	virtual ~OGLObj();

	RESULT ReleaseOGLBuffers();
	GLushort GetOGLPrecision();

	// This needs to be called from the sub-class constructor
	// or externally from the object (TODO: factory class needed)
	virtual RESULT OGLInitialize();
	RESULT UpdateOGLBuffers();
	virtual RESULT UpdateBuffers() override;

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	//virtual RESULT Render() {
	virtual RESULT Render();
	virtual RESULT Update();

	virtual RESULT RenderOGLBoundingVolume();
	virtual RESULT UpdateOGLBoundingVolume();

	OGLTexture *GetOGLTextureBump();
	OGLTexture *GetOGLTextureAmbient();
	OGLTexture *GetOGLTextureDiffuse();
	OGLTexture *GetOGLTextureSpecular();
	OGLTexture* GetOGLTextureDisplacement();
	
	OGLObj *GetOGLBoundingVolume();

	// TODO: Do we want to keep this - maybe move upstream
	RESULT SetOGLProgramPreCallback(std::function<RESULT(OGLProgram*, void*)> fnOGLProgramPreCallback);
	std::function<RESULT(OGLProgram*, void*)> GetOGLProgramPreCallback();

	RESULT SetOGLProgramPostCallback(std::function<RESULT(OGLProgram*, void*)> fnOGLProgramPostCallback);
	std::function<RESULT(OGLProgram*, void*)> GetOGLProgramPostCallback();

protected:
	GLuint m_hVAO;		// vertex array object
	GLuint m_hVBO;		// vertex buffer object
	GLuint m_hIBO;		// index buffer object

	OGLImp *m_pParentImp;

private:
	OGLObj *m_pOGLBoundingVolume;

	std::function<RESULT(OGLProgram*, void*)> m_fnOGLProgramPreCallback;
	std::function<RESULT(OGLProgram*, void*)> m_fnOGLProgramPostCallback;
};

#endif // ! OGL_OBJ_H_
