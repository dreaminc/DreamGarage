#include "OGLProgramFactory.h"

#include "OGLProgramMinimal.h"
#include "OGLProgramSkybox.h"
#include "OGLProgramBlinnPhong.h"
#include "OGLProgramMinimalTexture.h"

OGLProgram *OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_TYPE type, OpenGLImp *pParentImp, version versionOGL) {
	OGLProgram *pOGLProgram = nullptr;
	RESULT r = R_PASS;

	switch (type) {
		case OGLPROGRAM_MINIMAL: {
			pOGLProgram = new OGLProgramMinimal(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"minimal.vert", L"minimal.frag", versionOGL), "Failed to initialize OGL minimal Program");
		} break;

		case OGLPROGRAM_MINIMAL_TEXTURE: {
			pOGLProgram = new OGLProgramMinimalTexture(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"minimalTexture.vert", L"minimalTexture.frag", versionOGL), "Failed to initialize OGL minimal texture Program");
		} break;
		
		case OGLPROGRAM_SKYBOX: {
			pOGLProgram = new OGLProgramSkybox(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"skybox.vert", L"skybox.frag", versionOGL), "Failed to initialize OGL skybox Program");
		} break;

		
		case OGLPROGRAM_BLINNPHONG: {
			pOGLProgram = new OGLProgramBlinnPhong(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blinnPhong.vert", L"blinnPhong.frag", versionOGL), "Failed to initialize OGL blinnPhong Program");
		} break;

		/*
		case OGLPROGRAM_BLINNPHONG_TEXTURE: {
			pOGLProgram = new OGLProgramMinimal(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blinnPhongTexture.vert", L"blinnPhongTexture.frag", versionOGL), "Failed to initialize OGL blinnPhongTexture Program");
		} break;
		*/

		case OGLPROGRAM_CUSTOM:
		case OGLPROGRAM_INVALID: 
		default: {
			DEBUG_LINEOUT("OGLProgram of type %d not supported", type);
		} break;
	}

Success:
	return pOGLProgram;

Error:
	if (pOGLProgram != nullptr) {
		delete pOGLProgram;
		pOGLProgram = nullptr;
	}

	return nullptr;
}