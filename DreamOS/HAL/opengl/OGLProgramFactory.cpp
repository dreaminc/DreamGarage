#include "OGLProgramFactory.h"

#include "OGLProgramMinimal.h"
#include "OGLProgramSkybox.h"
#include "OGLProgramSkyboxScatter.h"
#include "OGLProgramBlinnPhong.h"
#include "OGLProgramBlinnPhongShadow.h"
#include "OGLProgramMinimalTexture.h"
#include "OGLProgramFlat.h"
#include "OGLProgramBlinnPhongTexture.h"
#include "OGLProgramBlinnPhongTextureShadow.h"
#include "OGLProgramBlinnPhongTextureBump.h"
#include "OGLProgramTextureBitBlit.h"
#include "OGLProgramShadowDepth.h"
#include "OGLProgramEnvironmentObjects.h"

std::shared_ptr<ProgramNode> OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_TYPE type, OpenGLImp *pParentImp, version versionOGL) {
	std::shared_ptr<OGLProgram> pOGLProgram = nullptr;
	RESULT r = R_PASS;

	switch (type) {
		case OGLPROGRAM_MINIMAL: {
			pOGLProgram = std::make_shared<OGLProgramMinimal>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"minimal.vert", L"minimal.frag", versionOGL), "Failed to initialize OGL minimal Program");
		} break;

		case OGLPROGRAM_MINIMAL_TEXTURE: {
			pOGLProgram = std::make_shared<OGLProgramMinimalTexture>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"minimalTexture.vert", L"minimalTexture.frag", versionOGL), "Failed to initialize OGL minimal texture Program");
		} break;
		
		case OGLPROGRAM_SKYBOX: {
			pOGLProgram = std::make_shared<OGLProgramSkybox>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"skybox.vert", L"skybox.frag", versionOGL), "Failed to initialize OGL skybox Program");
		} break;

		case OGLPROGRAM_SKYBOX_SCATTER: {
			pOGLProgram = std::make_shared<OGLProgramSkyboxScatter>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"skyboxScatter.vert", L"skyboxScatter.frag", versionOGL), "Failed to initialize OGL skybox Program");
		} break;
		
		case OGLPROGRAM_BLINNPHONG: {
			pOGLProgram = std::make_shared<OGLProgramBlinnPhong>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blinnPhong.vert", L"blinnPhong.frag", versionOGL), "Failed to initialize OGL blinnPhong Program");
		} break;

		case OGLPROGRAM_BLINNPHONG_SHADOW: {
			pOGLProgram = std::make_shared<OGLProgramBlinnPhongShadow>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blinnPhongShadow.vert", L"blinnPhongShadow.frag", versionOGL), "Failed to initialize OGL blinnPhong Program");
		} break;

		case OGLPROGRAM_BLINNPHONG_TEXTURE: {
			pOGLProgram = std::make_shared<OGLProgramBlinnPhongTexture>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blinnPhongTexture.vert", L"blinnPhongTexture.frag", versionOGL), "Failed to initialize OGL blinnPhongTexture Program");
		} break;

		case OGLPROGRAM_BLINNPHONG_TEXTURE_SHADOW: {
			pOGLProgram = std::make_shared<OGLProgramBlinnPhongTextureShadow>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blinnPhongTextureShadow.vert", L"blinnPhongTextureShadow.frag", versionOGL), "Failed to initialize OGL blinnPhongTexture Program");
		} break;

		case OGLPROGRAM_BLINNPHONG_TEXTURE_BUMP: {
			pOGLProgram = std::make_shared<OGLProgramBlinnPhongTextureBump>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blinnPhongTextureBump.vert", L"blinnPhongTextureBump.frag", versionOGL), "Failed to initialize OGL blinnPhongTextureBump Program");
		} break;

		case OGLPROGRAM_TEXTURE_BITBLIT: {
			pOGLProgram = std::make_shared<OGLProgramTextureBitBlit>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"TextureBitBlit.vert", L"TextureBitBlit.frag", versionOGL), "Failed to initialize OGL minimal texture Program");
		} break;

		case OGLPROGRAM_FLAT: {
			pOGLProgram = std::make_shared<OGLProgramFlat>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"flat.vert", L"flat.frag", versionOGL), "Failed to initialize OGL minimal texture Program");
		} break;

		case OGLPROGRAM_SHADOW_DEPTH: {
			pOGLProgram = std::make_shared<OGLProgramShadowDepth>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"ShadowDepth.vert", L"ShadowDepth.frag", versionOGL), "Failed to initialize OGL minimal texture Program");
		} break;

		case OGLPROGRAM_ENVIRONMENT_OBJECTS: {
			pOGLProgram = std::make_shared<OGLProgramEnvironmentObjects>(pParentImp);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"EnvironmentObjects.vert", L"EnvironmentObjects.frag", versionOGL), "Failed to initialize OGL EnvironmentObjects Program");
		} break;

		case OGLPROGRAM_CUSTOM:
		case OGLPROGRAM_INVALID: 
		default: {
			DEBUG_LINEOUT("OGLProgram of type %d not supported", type);
		} break;
	}

	// TODO: Move this somewhere better
	CR(pOGLProgram->SetupConnections());


//Success:
	return pOGLProgram;

Error:
	if (pOGLProgram != nullptr) {
		pOGLProgram = nullptr;
	}

	return nullptr;
}