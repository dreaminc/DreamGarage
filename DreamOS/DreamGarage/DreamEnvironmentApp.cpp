#include "DreamEnvironmentApp.h"
#include "DreamOS.h"

#include "HAL/opengl/OGLObj.h"
#include "HAL/opengl/OGLProgramStandard.h"
#include "HAL/opengl/OGLProgramScreenFade.h"
#include "HAL/SkyboxScatterProgram.h"

#include "Primitives/user.h"

#include "Sandbox/CommandLineManager.h"
#include "Core/Utilities.h"

#include "DreamUserApp.h"

#include "Scene/ObjectStoreNode.h"

DreamEnvironmentApp::DreamEnvironmentApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamEnvironmentApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

RESULT DreamEnvironmentApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	auto pDreamOS = GetDOS();

	m_lightIntensity = 1.0f;
	m_directionalIntensity = 1.0f;

	std::shared_ptr<OGLObj> pOGLObj = nullptr;
	HMD *pHMD = nullptr;
	bool fShowModels = true;

	//TODO: way to change environment after initialization
	std::string strEnvironmentPath = "default";

#ifndef PRODUCTION_BUILD
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	strEnvironmentPath = pCommandLineManager->GetParameterValue("environment.path");
#endif

	// One strong "SUN" directional light, and a second dimmer "ambient" light from the opposite direction
	float downwardAngle = 45.0f * (float)M_PI / 180.0f;

	vector vLight1 = vector(1.0f, -0.5f, 1.0f);
	vector vLight2 = vector(-1.0f, -0.5f, 1.0f);
	vector vLight3 = vector(0.0f, -0.5f, -1.0f);

	vector vLight4 = vector(0.0f, 1.0f, 0.0f);

	m_pDirectionalSunLight = pDreamOS->AddLight(LIGHT_DIRECTIONAL, m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLight1);
	m_pDirectionalAmbientLight = pDreamOS->AddLight(LIGHT_DIRECTIONAL, m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLight2);
	pDreamOS->AddLight(LIGHT_DIRECTIONAL, m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLight3);

	pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.2f * m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vLight4);

	//pDirectionalLight->EnableShadows();
	
	m_pSceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pSceneGraph, "Failed to allocate Scene Graph");
	CB(m_pSceneGraph->incRefCount());
	
	pHMD = pDreamOS->GetHMD();

	if (pHMD != nullptr) {
		if (pHMD->GetDeviceType() == HMDDeviceType::META) {
			fShowModels = false;
		}
	}

#ifdef _DEBUG
	fShowModels = false;
#endif

	if (fShowModels) {
		CR(LoadAllEnvironments());
	}

	// TODO: Add a way to connect a program composite directly to node (composite node?)
	CR(m_pSceneGraph->PushObject(GetComposite()));

Error:
	return r;
}

RESULT DreamEnvironmentApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamEnvironmentApp::Update(void *pContext) {
	RESULT r = R_PASS;

	//for (auto pProgram : m_skyboxPrograms) {
	//	pProgram->SetSunDirection(m_vSunDirection);
	//}

	if(m_pUserApp == nullptr) {
		auto userUIDs = GetDOS()->GetAppUID("DreamUserApp");

		CB(userUIDs.size() == 1);
		auto userUID = userUIDs[0];

		m_pUserApp = dynamic_cast<DreamUserApp*>(GetDOS()->CaptureApp(userUID, this));
		CN(m_pUserApp);
	}

Error:
	return r;
}

RESULT DreamEnvironmentApp::Shutdown(void *pContext) {
	return R_PASS;
}

DreamEnvironmentApp* DreamEnvironmentApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamEnvironmentApp *pDreamApp = new DreamEnvironmentApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamEnvironmentApp::PositionEnvironment(environment::type type, std::shared_ptr<model> pModel) {
	RESULT r = R_PASS;

	m_ptSceneOffset = point(0.0f, 0.0f, 0.0f);
	m_sceneScale = 0.1f;

	if (type == environment::ISLAND) {
		m_ptSceneOffset = point(90.0f, -5.0f, -25.0f);
		m_sceneScale = 0.1f;

	}
	else {
		m_ptSceneOffset = point(0.0f, -1.20f, 0.0f);
		m_sceneScale = m_environmentSceneScale;
		GetComposite()->RotateYByDeg(90.0f);
	}
	//*/

	//GetComposite()->SetPosition(m_ptSceneOffset);
	//GetComposite()->SetScale(m_sceneScale);
	pModel->SetPosition(m_ptSceneOffset);
	pModel->SetScale(m_sceneScale);

	return r;

}

RESULT DreamEnvironmentApp::LoadAllEnvironments() {
	RESULT r = R_PASS;

	for (auto& filenamePair : m_environmentFilenames) {

		//TODO: with unique environment shader, change this to MakeModel
		std::shared_ptr<model> pModel = GetComposite()->AddModel(filenamePair.second);
		CN(pModel);

		m_environmentModels[filenamePair.first] = pModel;
		pModel->SetVisible(false);
		PositionEnvironment(filenamePair.first, pModel);
	}

Error:
	return r;
}

RESULT DreamEnvironmentApp::SetCurrentEnvironment(environment::type type) {
	m_pCurrentEnvironmentModel = m_environmentModels[type];
	m_currentType = type;
	return R_PASS;
}

RESULT DreamEnvironmentApp::SetSkyboxPrograms(std::vector<SkyboxScatterProgram*> pPrograms) {
	m_skyboxPrograms = pPrograms;
	return R_PASS;
}

RESULT DreamEnvironmentApp::SetScreenFadeProgram(OGLProgramScreenFade* pFadeProgram) {
	m_pFadeProgram = pFadeProgram;
	return R_PASS;
}

RESULT DreamEnvironmentApp::HideEnvironment(void *pContext) {
	RESULT r = R_PASS;

	auto fnOnFadeOut = [&](void *pContext) {
		m_pCurrentEnvironmentModel->SetVisible(false);
		m_pFadeProgram->FadeIn();

		// Assuming we want to show welcome back quad here
		m_pUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::SIGN_IN);
		m_pUserApp->ShowMessageQuad();

		return R_PASS;
	};

	CNR(m_pFadeProgram, R_SKIPPED);

	m_pFadeProgram->FadeOut(fnOnFadeOut);

Error:
	return r;
}

RESULT DreamEnvironmentApp::ShowEnvironment(void *pContext) {
	RESULT r = R_PASS;

	auto fnOnFadeOut = [&](void *pContext) {
		if (m_pCurrentEnvironmentModel != nullptr) {
			m_pCurrentEnvironmentModel->SetVisible(true);
			m_pUserApp->HideMessageQuad();
		}

		m_pFadeProgram->FadeIn();

		return R_PASS;
	};

	float fadeProgress = 0.0f;
	CNR(m_pFadeProgram, R_SKIPPED);

	fadeProgress = m_pFadeProgram->GetFadeProgress();
	
	if (fadeProgress == 1.0f) {
		m_pFadeProgram->FadeOut(fnOnFadeOut);
	}
	else if (fadeProgress == 0.0f) {
		if (m_pCurrentEnvironmentModel != nullptr) {
			m_pCurrentEnvironmentModel->SetVisible(true);
		}

		m_pFadeProgram->FadeIn();
	}

Error:
	return r;
}

RESULT DreamEnvironmentApp::FadeIn() {
	RESULT r = R_PASS;

	CNR(m_pFadeProgram, R_SKIPPED);
	m_pFadeProgram->FadeIn();

Error:
	return r;
}


RESULT DreamEnvironmentApp::SwitchToEnvironment(environment::type type) {
	RESULT r = R_PASS;

	m_currentType = type;

	auto fnOnFadeOut = [&](void *pContext) {

		m_pCurrentEnvironmentModel->SetVisible(false);
		m_pCurrentEnvironmentModel = m_environmentModels[m_currentType];
		m_pCurrentEnvironmentModel->SetVisible(true);

		m_pFadeProgram->FadeIn();

		return R_PASS;
	};

	CNR(m_pFadeProgram, R_SKIPPED);

	m_pFadeProgram->FadeOut(fnOnFadeOut);

Error:
	return r;
}

RESULT DreamEnvironmentApp::GetSharedScreenPosition(point& ptPosition, quaternion& qOrientation, float& scale) {
	RESULT r = R_PASS;

	switch (m_currentType) {
	case environment::ISLAND: {
		// legacy
		ptPosition = point(0.0f, 2.0f, -2.0f);
		qOrientation = quaternion();
		scale = 1.0f;
	} break;
	case environment::CAVE: {
		ptPosition = point(m_tableLength*0.75f, 1.25f, 0.0f);
		qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, -90.0f * (float)M_PI / 180.0f, 0.0f);
		scale = 0.75f;
	} break;
	}

//Error:
	return r;
}

RESULT DreamEnvironmentApp::GetEnvironmentSeatingPositionAndOrientation(point& ptPosition, quaternion& qOrientation, int seatIndex) {
	RESULT r = R_PASS;

	CBM(seatIndex < m_maxSeatingIndex && seatIndex >= 0, "Peer index %d not supported by client", seatIndex);

	// position
	switch (seatIndex) {
	case 0: {
		ptPosition = point(-m_tableLength / 2.0f, m_tableHeight, -(m_tableWidth - 1.5f) / 2.0f);
		qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle - m_frontAngle, 0.0f); break;
	} break;

	case 1: {
		ptPosition = point(-m_tableLength / 2.0f, m_tableHeight, (m_tableWidth - 1.5f) / 2.0f);
		qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle + m_frontAngle, 0.0f); break;
	} break;

	case 2: {
		ptPosition = point(-m_tableLength / 4.0f, m_tableHeight, -(m_tableWidth) / 2.0f);
		qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle - m_middleAngle, 0.0f); break;
	} break;

	case 3: {
		ptPosition = point(-m_tableLength / 4.0f, m_tableHeight, (m_tableWidth) / 2.0f);
		qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle + m_middleAngle, 0.0f); break;
	} break;

	case 4: {
		ptPosition = point(0.0f, m_tableHeight, -(m_tableWidth + 0.5f) / 2.0f);
		qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle - m_backAngle, 0.0f); break;
	} break;

	case 5: {
		ptPosition = point(0.0f, m_tableHeight, (m_tableWidth + 0.5f) / 2.0f);
		qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle + m_backAngle, 0.0f); break;
	} break;
	}

Error:
	return r;
}

quaternion DreamEnvironmentApp::GetUIOffsetOrientation(int seatIndex) {
	RESULT r = R_PASS;

	float angle;
	float offsetAngle = 90.0f * (float)M_PI / 180.0f;
	switch (seatIndex) {
	case 0: {
		angle = 2.0f * (m_frontAngle - offsetAngle);
	} break;
	case 1: {
		angle = -2.0f * (m_frontAngle - offsetAngle);
	} break;
	case 2: {
		angle = 2.0f * (m_middleAngle - offsetAngle);
	} break;
	case 3: {
		angle = -2.0f * (m_middleAngle - offsetAngle);
	} break;
	case 4: {
		angle = 2.0f * (m_backAngle - offsetAngle);
	} break;
	case 5: {
		angle = -2.0f * (m_backAngle - offsetAngle);
	} break;
	}

	return quaternion::MakeQuaternionWithEuler(0.0f, angle, 0.0f);
}
