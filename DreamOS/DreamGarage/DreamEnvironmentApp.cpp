#include "DreamEnvironmentApp.h"
#include "DreamOS.h"

#include "HAL/opengl/OGLObj.h"
#include "HAL/opengl/OGLProgramEnvironmentObjects.h"

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

	std::shared_ptr<OGLObj> pOGLObj = nullptr;

	//TODO: way to change environment after initialization
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	std::string strEnvironmentPath = pCommandLineManager->GetParameterValue("environment.path");

	//TODO: environments probably won't all have the same lighting
	if (strEnvironmentPath == "default") {
		auto pDirectionalLight = pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.0f, point(0.0f, 10.0f, 2.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, 0.0f));
		pDirectionalLight->EnableShadows();

		pDreamOS->AddLight(LIGHT_POINT, 5.0f, point(20.0f, 7.0f, -40.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	}
	else {
		m_lightIntensity = 15.0f;
	}

	pDreamOS->AddLight(LIGHT_POINT, m_lightIntensity, point(5.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	pDreamOS->AddLight(LIGHT_POINT, m_lightIntensity, point(-5.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	pDreamOS->AddLight(LIGHT_POINT, m_lightIntensity, point(-5.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	pDreamOS->AddLight(LIGHT_POINT, m_lightIntensity, point(5.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));


	if (strEnvironmentPath == "default") {
		m_ptSceneOffset = point(90.0f, -5.0f, -25.0f);
		m_sceneScale = 0.1f;

		//TODO: may need a way to load multiple files for the environment in a more general way
		auto pModel = GetComposite()->AddModel(L"\\FloatingIsland\\env.obj");
		CN(pModel);
		auto pRiver = GetComposite()->AddModel(L"\\FloatingIsland\\river.obj");
		CN(pRiver);
		auto pClouds = GetComposite()->AddModel(L"\\FloatingIsland\\clouds.obj");
		CN(pClouds);
		pClouds->SetMaterialAmbient(0.8f);

		pOGLObj = std::dynamic_pointer_cast<OGLObj>(pRiver->GetChildren()[0]);
		if (pOGLObj != nullptr) {
			pOGLObj->SetOGLProgramPreCallback(
				[](OGLProgram* pOGLProgram, void *pContext) {
				// Do some stuff pre-render
				OGLProgramEnvironmentObjects *pOGLEnvironmentProgram = dynamic_cast<OGLProgramEnvironmentObjects*>(pOGLProgram);
				if (pOGLEnvironmentProgram != nullptr) {
					pOGLEnvironmentProgram->SetRiverAnimation(true);
				}
				return R_PASS;
			}
			);

			pOGLObj->SetOGLProgramPostCallback(
				[](OGLProgram* pOGLProgram, void *pContext) {
				// Do some stuff post

				OGLProgramEnvironmentObjects *pOGLEnvironmentProgram = dynamic_cast<OGLProgramEnvironmentObjects*>(pOGLProgram);
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
		m_ptSceneOffset = point(0.0f, -5.0f, 0.0f);
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
	return R_PASS;
}

RESULT DreamEnvironmentApp::Shutdown(void *pContext) {
	return R_PASS;
}

DreamEnvironmentApp* DreamEnvironmentApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamEnvironmentApp *pDreamApp = new DreamEnvironmentApp(pDreamOS, pContext);
	return pDreamApp;
}
