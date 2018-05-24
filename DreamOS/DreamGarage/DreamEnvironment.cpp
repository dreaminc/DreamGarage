#include "DreamEnvironment.h"
#include "DreamOS.h"

#include "HAL/opengl/OGLObj.h"
#include "HAL/opengl/OGLProgramEnvironmentObjects.h"

#include "Sandbox/CommandLineManager.h"
#include "Core/Utilities.h"


DreamEnvironment::DreamEnvironment(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamEnvironment>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

RESULT DreamEnvironment::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	auto pDreamOS = GetDOS();

	float intensity = 1.0f;

	std::shared_ptr<OGLObj> pOGLObj = nullptr;
	point ptSceneOffset = point(90, -5, -25);
	float sceneScale = 0.1f;
	vector vSceneEulerOrientation = vector(0.0f, 0.0f, 0.0f);

	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	std::string strEnvironmentPath = pCommandLineManager->GetParameterValue("environment.path");

	if (strEnvironmentPath == "default") {
		auto pDirectionalLight = pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.0f, point(0.0f, 10.0f, 2.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, 0.0f));
		pDirectionalLight->EnableShadows();

		pDreamOS->AddLight(LIGHT_POINT, 5.0f, point(20.0f, 7.0f, -40.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	}
	else {
		intensity = 15.0f;
	}

	pDreamOS->AddLight(LIGHT_POINT, intensity, point(5.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	pDreamOS->AddLight(LIGHT_POINT, intensity, point(-5.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	pDreamOS->AddLight(LIGHT_POINT, intensity, point(-5.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	pDreamOS->AddLight(LIGHT_POINT, intensity, point(5.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	bool fShowModels = true;
	auto pHMD = pDreamOS->GetHMD();
	if (pHMD != nullptr) {
		if (pHMD->GetDeviceType() == HMDDeviceType::META) {
			fShowModels = false;
		}
	}

	CBR(fShowModels, R_SKIPPED);

	if (strEnvironmentPath == "default") {
		model* pModel = pDreamOS->AddModel(L"\\FloatingIsland\\env.obj");
		pModel->SetPosition(ptSceneOffset);
		pModel->SetScale(sceneScale);

		model* pRiver = pDreamOS->AddModel(L"\\FloatingIsland\\river.obj");
		pRiver->SetPosition(ptSceneOffset);
		pRiver->SetScale(sceneScale);

		model* pClouds = pDreamOS->AddModel(L"\\FloatingIsland\\clouds.obj");
		pClouds->SetPosition(ptSceneOffset);
		pClouds->SetScale(sceneScale);
		//pModel->SetEulerOrientation(vSceneEulerOrientation);
		//pClouds->SetVisible(false);

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
		model *pModel = pDreamOS->AddModel(util::StringToWideString(strEnvironmentPath));
		pModel->RotateXByDeg(-90.0f);
		pModel->RotateYByDeg(90.0f);
		//TODO: in theory this should be 1.0f if the models we get are in meters
		pModel->SetScale(sceneScale);
		pModel->SetPosition(point(0.0f, -5.0f, 0.0f));
	}
	//*/

Error:
	return r;
}

RESULT DreamEnvironment::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamEnvironment::Update(void *pContext) {
	return R_PASS;
}

RESULT DreamEnvironment::Shutdown(void *pContext) {
	return R_PASS;
}

DreamEnvironment* DreamEnvironment::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamEnvironment *pDreamApp = new DreamEnvironment(pDreamOS, pContext);
	return pDreamApp;
}
