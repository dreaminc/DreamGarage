#include "DreamEnvironmentApp.h"
#include "DreamOS.h"

#include "HAL/opengl/OGLObj.h"
#include "HAL/opengl/OGLProgramStandard.h"
#include "HAL/SkyboxScatterProgram.h"

#include "Sandbox/CommandLineManager.h"
#include "Core/Utilities.h"

DreamEnvironmentApp::DreamEnvironmentApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamEnvironmentApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

RESULT DreamEnvironmentApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	auto pDreamOS = GetDOS();

	m_ptSceneOffset = point(0.0f, 0.0f, 0.0f);
	m_sceneScale = 1.0f;
	m_lightIntensity = 1.0f;
	m_directionalIntensity = 1.0f;

	std::shared_ptr<OGLObj> pOGLObj = nullptr;

	//TODO: way to change environment after initialization
	std::string strEnvironmentPath = "default";

#ifndef PRODUCTION_BUILD
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	strEnvironmentPath = pCommandLineManager->GetParameterValue("environment.path");
#endif

	//TODO: environments probably won't all have the same lighting
	if (strEnvironmentPath == "default") {
		
		// One strong "SUN" directional light, and a second dimmer "ambient" light from the opposite direction
		vector vLightVector = m_vSunDirection;

		m_pDirectionalSunLight = pDreamOS->AddLight(LIGHT_DIRECTIONAL, m_directionalIntensity, point(0.0f, 10.0f, 2.0f), color(COLOR_WHITE), color(COLOR_WHITE), (vector)(-1.0f * vLightVector));
		m_pDirectionalAmbientLight = pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.35f * m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLightVector);

		//pDirectionalLight->EnableShadows();

		//pDreamOS->AddLight(LIGHT_POINT, 5.0f, point(20.0f, 7.0f, -40.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	}
	else {
		m_lightIntensity = 15.0f;
	}

	//pDreamOS->AddLight(LIGHT_POINT, m_lightIntensity, point(5.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	//pDreamOS->AddLight(LIGHT_POINT, m_lightIntensity, point(-5.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	//pDreamOS->AddLight(LIGHT_POINT, m_lightIntensity, point(-5.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	//pDreamOS->AddLight(LIGHT_POINT, m_lightIntensity, point(5.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));


	if (strEnvironmentPath == "default") {
		m_ptSceneOffset = point(90.0f, -5.0f, -25.0f);
		m_sceneScale = 0.1f;

		//auto pModel = GetComposite()->AddModel(L"\\Cave\\cave_no_water_ib.fbx");
		//CN(pModel);

		//TODO: may need a way to load multiple files for the environment in a more general way
		auto pModel = GetComposite()->AddModel(L"\\FloatingIsland\\env.obj");
		CN(pModel);

		//auto pRiver = GetComposite()->AddModel(L"\\FloatingIsland\\river.obj");
		//CN(pRiver);

		auto pClouds = GetComposite()->AddModel(L"\\FloatingIsland\\clouds.obj");
		CN(pClouds);
		pClouds->SetMaterialAmbient(0.8f, true);
		pClouds->translateZ(-50.0f);

		//pOGLObj = std::dynamic_pointer_cast<OGLObj>(pRiver->GetChildren()[0]);
		if (pOGLObj != nullptr) {
			pOGLObj->SetOGLProgramPreCallback(
				[](OGLProgram* pOGLProgram, void *pContext) {
				// Do some stuff pre-render
				OGLProgramStandard *pOGLEnvironmentProgram = dynamic_cast<OGLProgramStandard*>(pOGLProgram);
				if (pOGLEnvironmentProgram != nullptr) {
					pOGLEnvironmentProgram->SetRiverAnimation(true);
				}
				return R_PASS;
			}
			);

			pOGLObj->SetOGLProgramPostCallback(
				[](OGLProgram* pOGLProgram, void *pContext) {
				// Do some stuff post

				OGLProgramStandard *pOGLEnvironmentProgram = dynamic_cast<OGLProgramStandard*>(pOGLProgram);
				if (pOGLEnvironmentProgram != nullptr) {
					pOGLEnvironmentProgram->SetRiverAnimation(false);
				}
				return R_PASS;
			}
			);
		}
	}
	else {
		auto pModel = GetComposite()->AddModel(util::StringToWideString(strEnvironmentPath));
		pModel->RotateXByDeg(-90.0f);
		pModel->RotateYByDeg(90.0f);
		//TODO: in theory this should be 1.0f if the models we get are in meters
		//m_ptSceneOffset = point(0.0f, -5.0f, 0.0f);
	}
	//*/

	GetComposite()->SetPosition(m_ptSceneOffset);
	GetComposite()->SetScale(m_sceneScale);

Error:
	return r;
}

RESULT DreamEnvironmentApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamEnvironmentApp::Update(void *pContext) {

	for (auto pProgram : m_skyboxPrograms) {
		pProgram->SetSunDirection(m_vSunDirection);
	}

	return R_PASS;
}

RESULT DreamEnvironmentApp::Shutdown(void *pContext) {
	return R_PASS;
}

DreamEnvironmentApp* DreamEnvironmentApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamEnvironmentApp *pDreamApp = new DreamEnvironmentApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamEnvironmentApp::SetSkyboxPrograms(std::vector<SkyboxScatterProgram*> pPrograms) {
	m_skyboxPrograms = pPrograms;
	return R_PASS;
}
