#include "DreamEnvironmentApp.h"
#include "DreamOS.h"

#include "HAL/opengl/OGLObj.h"
#include "HAL/opengl/OGLProgramStandard.h"
#include "HAL/opengl/OGLProgramScreenFade.h"
#include "HAL/SkyboxScatterProgram.h"

#include "Primitives/user.h"

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

	m_lightIntensity = 1.0f;
	m_directionalIntensity = 2.0f;

	std::shared_ptr<OGLObj> pOGLObj = nullptr;

	//TODO: way to change environment after initialization
	std::string strEnvironmentPath = "default";

#ifndef PRODUCTION_BUILD
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	strEnvironmentPath = pCommandLineManager->GetParameterValue("environment.path");
#endif

	// One strong "SUN" directional light, and a second dimmer "ambient" light from the opposite direction
	m_pDirectionalSunLight = pDreamOS->AddLight(LIGHT_DIRECTIONAL, m_directionalIntensity, point(0.0f, 10.0f, 2.0f), color(COLOR_WHITE), color(COLOR_WHITE), (vector)(-1.0f * m_vSunDirection));
	m_pDirectionalAmbientLight = pDreamOS->AddLight(LIGHT_DIRECTIONAL, 0.35f * m_directionalIntensity, point(0.0f, 0.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), m_vSunDirection);

	//pDirectionalLight->EnableShadows();

	CR(LoadAllEnvironments());

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

RESULT DreamEnvironmentApp::PositionEnvironment(EnvironmentType type, std::shared_ptr<model> pModel) {
	RESULT r = R_PASS;

	m_ptSceneOffset = point(0.0f, 0.0f, 0.0f);
	m_sceneScale = 0.1f;

	if (type == ISLAND) {
		m_ptSceneOffset = point(90.0f, -5.0f, -25.0f);
		m_sceneScale = 0.1f;

	}
	else {
		m_sceneScale = m_environmentSceneScale;
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

RESULT DreamEnvironmentApp::SetCurrentEnvironment(EnvironmentType type) {
	m_pCurrentEnvironmentModel = m_environmentModels[type];
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
		m_pCurrentEnvironmentModel->SetVisible(true);
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
		m_pCurrentEnvironmentModel->SetVisible(true);
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


RESULT DreamEnvironmentApp::SwitchToEnvironment(EnvironmentType type) {
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

RESULT DreamEnvironmentApp::SeatUser(point& ptPosition, quaternion& qOrientation, int seatIndex) {
	RESULT r = R_PASS;

	CBM(seatIndex < m_maxSeatingIndex && seatIndex >= 0, "Peer index %d not supported by client", seatIndex);

	// position
	switch (seatIndex) {
	case 0: ptPosition = point(-m_tableLength / 2.0f, m_tableHeight, -(m_tableWidth - 1.5f) / 2.0f); break;
	case 1: ptPosition = point(-m_tableLength / 2.0f, m_tableHeight, (m_tableWidth - 1.5f) / 2.0f); break;
	case 2: ptPosition = point(-m_tableLength / 4.0f, m_tableHeight, -(m_tableWidth) / 2.0f); break;
	case 3: ptPosition = point(-m_tableLength / 4.0f, m_tableHeight, (m_tableWidth) / 2.0f); break;
	case 4: ptPosition = point(0.0f,					m_tableHeight, -(m_tableWidth + 0.5f) / 2.0f); break;
	case 5: ptPosition = point(0.0f,					m_tableHeight, (m_tableWidth + 0.5f) / 2.0f); break;
	}

	switch (seatIndex) {
	case 0: qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle - m_frontAngle, 0.0f); break;
	case 1: qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle + m_frontAngle, 0.0f); break;
	case 2: qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle - m_middleAngle, 0.0f); break;
	case 3: qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle + m_middleAngle, 0.0f); break;
	case 4: qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle - m_backAngle, 0.0f); break;
	case 5: qOrientation = quaternion::MakeQuaternionWithEuler(0.0f, m_baseTableAngle + m_backAngle, 0.0f); break;
	}

Error:
	return r;
}
