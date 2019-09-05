#ifndef OGL_SKYBOX_H_
#define OGL_SKYBOX_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLSkybox.h

// OGL Skybox
// TODO: Remove this potentially 

#include "OGLObj.h"
#include "OGLTexture.h"

#include "core/primitives/skybox.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLSkybox : public skybox, public OGLObj {
public:
	OGLSkybox(OGLImp *pParentImp);


public:
	/*
	RESULT OGLActivateCubeMapTexture();
	*/

	// Face culling
	
	///*
	//RESULT Render() {
	virtual RESULT Render() override;
	//*/
};
#pragma warning(pop)

#endif	// ! OGL_SKYBOX_H_