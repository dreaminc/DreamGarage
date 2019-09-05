#include "OGLProgramFactory.h"

#include "OGLProgramMinimal.h"
#include "OGLProgramReferenceGeometry.h"
#include "OGLProgramSkybox.h"
#include "OGLProgramSkyboxScatter.h"
#include "OGLProgramSkyboxScatterCube.h"
#include "OGLProgramBlinnPhong.h"
#include "OGLProgramBlinnPhongShadow.h"
#include "OGLProgramMinimalTexture.h"
#include "OGLProgramFlat.h"
#include "OGLProgramToon.h"
#include "OGLProgramReflection.h"
#include "OGLProgramRefraction.h"
#include "OGLProgramEnvironment.h"
#include "OGLProgramWater.h"
#include "OGLProgramBlinnPhongTexture.h"
#include "OGLProgramBlinnPhongTextureShadow.h"
#include "OGLProgramBlinnPhongTextureBump.h"
#include "OGLProgramBlinnPhongTextureBumpDisplacement.h"
#include "OGLProgramTextureBitBlit.h"
#include "OGLProgramShadowDepth.h"
#include "OGLProgramStandard.h"
#include "OGLProgramScreenQuad.h"
#include "OGLProgramIrradianceMap.h"
#include "OGLProgramCubemapConvolution.h"
#include "OGLProgramScreenFade.h"
#include "OGLProgramDepthPeel.h"
#include "OGLProgramBlendQuad.h"
#include "OGLProgramBlurQuad.h"
#include "OGLProgramUIStage.h"
#include "OGLProgramDebugOverlay.h"
#include "OGLProgramVisualizeNormals.h"
#include "OGLProgramBillboard.h"

// Deferred 
#include "OGLProgramGBuffer.h"
#include "OGLProgramSSAO.h"

const std::map<std::string, OGLPROGRAM_TYPE> OGLProgramFactory::m_OGLProgramNameType = {
	{ "minimal", OGLPROGRAM_MINIMAL },
	{ "minimal_texture", OGLPROGRAM_MINIMAL_TEXTURE },
	{ "skybox", OGLPROGRAM_SKYBOX },
	{ "skybox_scatter", OGLPROGRAM_SKYBOX_SCATTER },
	{ "skybox_scatter_cube", OGLPROGRAM_SKYBOX_SCATTER_CUBE},
	{ "blinnphong", OGLPROGRAM_BLINNPHONG },
	{ "blinnphong_shadow", OGLPROGRAM_BLINNPHONG_SHADOW },
	{ "blinnphong_texture", OGLPROGRAM_BLINNPHONG_TEXTURE },
	{ "blinnphong_texture_shadow", OGLPROGRAM_BLINNPHONG_TEXTURE_SHADOW },
	{ "blinnphong_texture_bump", OGLPROGRAM_BLINNPHONG_TEXTURE_BUMP },
	{ "blinnphong_texture_bump_displacement", OGLPROGRAM_BLINNPHONG_TEXTURE_BUMP_DISPLACEMENT },
	{ "texture_bitblit", OGLPROGRAM_TEXTURE_BITBLIT },
	{ "flat", OGLPROGRAM_FLAT },
	{ "toon", OGLPROGRAM_TOON },
	{ "reflection", OGLPROGRAM_REFLECTION },
	{ "refraction", OGLPROGRAM_REFRACTION },
	{ "environment", OGLPROGRAM_ENVIRONMENT },
	{ "water", OGLPROGRAM_WATER },
	{ "custom", OGLPROGRAM_CUSTOM },
	{ "shadow_depth", OGLPROGRAM_SHADOW_DEPTH },
	{ "reference", OGLPROGRAM_REFERENCE },
	{ "standard", OGLPROGRAM_STANDARD },
	{ "screenquad", OGLPROGRAM_SCREEN_QUAD },
	{ "irrandiance_map_lighting", OGLPROGRAM_IRRADIANCE_MAP },
	{ "cubemap_convolution", OGLPROGRAM_CUBEMAP_CONVOLUTION },
	{ "gbuffer", OGLPROGRAM_GBUFFER },
	{ "ssao", OGLPROGRAM_SSAO },
	{ "screenfade", OGLPROGRAM_SCREEN_FADE },
	{ "depthpeel", OGLPROGRAM_DEPTH_PEEL },
	{ "blendquad", OGLPROGRAM_BLEND_QUAD },
	{ "blur", OGLPROGRAM_BLUR_QUAD },
	{ "uistage", OGLPROGRAM_UI_STAGE },
	{ "debug_overlay", OGLPROGRAM_DEBUG_OVERLAY },
	{ "visualize_normals", OGLPROGRAM_VISUALIZE_NORMALS },
	{ "billboard", OGLPROGRAM_BILLBOARD },
	{ "invalid", OGLPROGRAM_INVALID }
};

OGLPROGRAM_TYPE OGLProgramFactory::OGLProgramTypeFromstring(std::string strProgramName) {
	if (m_OGLProgramNameType.count(strProgramName) > 0) {
		return m_OGLProgramNameType.at(strProgramName);
	}
	else {
		return OGLPROGRAM_INVALID;
	}
}

ProgramNode* OGLProgramFactory::MakeOGLProgram(OGLPROGRAM_TYPE type, OGLImp *pParentImp, version versionOGL, PIPELINE_FLAGS optFlags) {
	OGLProgram* pOGLProgram = nullptr;
	RESULT r = R_PASS;

	switch (type) {

		case OGLPROGRAM_STANDARD: {
			pOGLProgram = new OGLProgramStandard(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL environment Program");
		} break;


		case OGLPROGRAM_MINIMAL: {
			pOGLProgram = new OGLProgramMinimal(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"minimal.vert", L"minimal.frag", versionOGL), 
				"Failed to initialize OGL minimal Program");
		} break;

		case OGLPROGRAM_REFERENCE: {
			pOGLProgram = new OGLProgramReferenceGeometry(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"minimal.vert", L"minimal.frag", versionOGL), 
				"Failed to initialize OGL reference geometry Program");
		} break;

		case OGLPROGRAM_MINIMAL_TEXTURE: {
			pOGLProgram = new OGLProgramMinimalTexture(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL), 
				"Failed to initialize OGL minimal texture Program");
		} break;

		case OGLPROGRAM_UI_STAGE: {
			pOGLProgram = new OGLProgramUIStage(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"uistage.vert", L"uistage.frag", versionOGL),
				"Failed to initialize OGL minimal texture Program");
		} break;

		case OGLPROGRAM_SKYBOX: {
			pOGLProgram = new OGLProgramSkybox(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL), 
				"Failed to initialize OGL skybox Program");
		} break;

		case OGLPROGRAM_SKYBOX_SCATTER: {
			pOGLProgram = new OGLProgramSkyboxScatter(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL), 
				"Failed to initialize OGL skybox scatter Program");
		} break;

			
		case OGLPROGRAM_SKYBOX_SCATTER_CUBE: {
			pOGLProgram = new OGLProgramSkyboxScatterCube(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL skybox scatter cube Program");
		} break;
		
		case OGLPROGRAM_BLINNPHONG: {
			pOGLProgram = new OGLProgramBlinnPhong(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL), 
				"Failed to initialize OGL blinnPhong Program");
		} break;

		case OGLPROGRAM_BLINNPHONG_TEXTURE: {
			pOGLProgram = new OGLProgramBlinnPhongTexture(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL blinnPhongTexture Program");
		} break;

		case OGLPROGRAM_BLINNPHONG_SHADOW: {
			pOGLProgram = new OGLProgramBlinnPhongShadow(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blinnPhongShadow.vert", L"blinnPhongShadow.frag", versionOGL), 
				"Failed to initialize OGL blinnPhongShadow Program");
		} break;

		case OGLPROGRAM_BLINNPHONG_TEXTURE_SHADOW: {
			pOGLProgram = new OGLProgramBlinnPhongTextureShadow(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blinnPhongTextureShadow.vert", L"blinnPhongTextureShadow.frag", versionOGL), 
				"Failed to initialize OGL blinnPhongTextureShadow Program");
		} break;

		case OGLPROGRAM_BLINNPHONG_TEXTURE_BUMP: {
			pOGLProgram = new OGLProgramBlinnPhongTextureBump(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL), 
				"Failed to initialize OGL blinnPhongTextureBump Program");
		} break;

		case OGLPROGRAM_BLINNPHONG_TEXTURE_BUMP_DISPLACEMENT: {
			pOGLProgram = new OGLProgramBlinnPhongTextureBumpDisplacement(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL blinnPhongTextureBumpDisplacement Program");
		} break;

		case OGLPROGRAM_TEXTURE_BITBLIT: {
			pOGLProgram = new OGLProgramTextureBitBlit(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"TextureBitBlit.vert", L"TextureBitBlit.frag", versionOGL), 
				"Failed to initialize OGL bit blit Program");
		} break;

		case OGLPROGRAM_FLAT: {
			pOGLProgram = new OGLProgramFlat(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"flat.vert", L"flat.frag", versionOGL),
				"Failed to initialize OGL flat Program");
		} break;

		case OGLPROGRAM_TOON: {
			pOGLProgram = new OGLProgramToon(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL flat Program");
		} break;

		case OGLPROGRAM_REFLECTION: {
			pOGLProgram = new OGLProgramReflection(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL flat Program");
		} break;

		case OGLPROGRAM_REFRACTION: {
			pOGLProgram = new OGLProgramRefraction(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL refraction Program");
		} break;

		case OGLPROGRAM_ENVIRONMENT: {
			pOGLProgram = new OGLProgramEnvironment(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL environment Program");
		} break;

		case OGLPROGRAM_WATER: {
			pOGLProgram = new OGLProgramWater(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL water Program");
		} break;

		case OGLPROGRAM_SHADOW_DEPTH: {
			pOGLProgram = new OGLProgramShadowDepth(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"ShadowDepth.vert", L"ShadowDepth.frag", versionOGL), 
				"Failed to initialize OGL shadow depth Program");
		} break;

		case OGLPROGRAM_SCREEN_QUAD: {
			pOGLProgram = new OGLProgramScreenQuad(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL), 
				"Failed to initialize OGL screen quad texture Program");
		} break;

		case OGLPROGRAM_IRRADIANCE_MAP: {
			pOGLProgram = new OGLProgramIrradianceMap(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL irradiance map Program");
		} break;

		case OGLPROGRAM_CUBEMAP_CONVOLUTION: {
			pOGLProgram = new OGLProgramCubemapConvolution(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL irradiance map Program");
		} break;

		// Deferred

		case OGLPROGRAM_GBUFFER: {
			pOGLProgram = new OGLProgramGBuffer(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL irradiance map Program");
		} break;

		case OGLPROGRAM_SSAO: {
			pOGLProgram = new OGLProgramSSAO(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL irradiance map Program");
		} break;

		case OGLPROGRAM_SCREEN_FADE: {
			pOGLProgram = new OGLProgramScreenFade(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL), 
				"Failed to initialize OGL screen quad texture Program");
		} break;

		case OGLPROGRAM_DEPTH_PEEL: {
			pOGLProgram = new OGLProgramDepthPeel(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"depthpeel.vert", L"depthpeel.frag", versionOGL),
				"Failed to initialize OGL depth peel Program");
		} break;

		case OGLPROGRAM_BLEND_QUAD: {
			pOGLProgram = new OGLProgramBlendQuad(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blendquad.vert", L"blendquad.frag", versionOGL),
				"Failed to initialize OGL blend quad Program");
		} break;

		case OGLPROGRAM_BLUR_QUAD: {
			pOGLProgram = new OGLProgramBlurQuad(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"blurquad.vert", L"blurquad.frag", versionOGL),
				"Failed to initialize OGL blur quad Program");
		} break;

		case OGLPROGRAM_DEBUG_OVERLAY: {
			pOGLProgram = new OGLProgramDebugOverlay(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(L"minimal.vert", L"minimal.frag", versionOGL),
				"Failed to initialize OGL minimal Program");
		} break;

		case OGLPROGRAM_VISUALIZE_NORMALS: {
			pOGLProgram = new OGLProgramVisualizeNormals(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL visualize normals Program");
		} break;

		case OGLPROGRAM_BILLBOARD: {
			pOGLProgram = new OGLProgramBillboard(pParentImp, optFlags);
			CNM(pOGLProgram, "Failed to allocate OGLProgram");
			CRM(pOGLProgram->OGLInitialize(versionOGL),
				"Failed to initialize OGL billboard Program");
		} break;

		case OGLPROGRAM_CUSTOM:
		case OGLPROGRAM_INVALID: 
		default: {
			DEBUG_LINEOUT("OGLProgram of type %d not supported", type);
			return nullptr;
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