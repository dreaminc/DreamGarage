#ifndef DREAM_ENVIRONMENT_H_
#define DREAM_ENVIRONMENT_H_

#include "./RESULT/EHM.h"
#include "DreamApp.h"
#include "Primitives/point.h"

#include <map>

class DreamOS;
class quad;
class model;
class light;
class SkyboxScatterProgram;
//TODO: move to proxy?
class OGLProgramScreenFade;
class user;

typedef enum EnvironmentType {
	LOBBY,
	CAVE,
	ISLAND
} ENVIRONMENT_TYPE;

class DreamEnvironmentApp : public DreamApp<DreamEnvironmentApp> {
	friend class DreamAppManager;

	// DreamApp
public:
	DreamEnvironmentApp(DreamOS *pDreamOS, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamEnvironmentApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:
	RESULT PositionEnvironment(EnvironmentType type, std::shared_ptr<model> pModel);
	RESULT LoadAllEnvironments();
	RESULT SetCurrentEnvironment(EnvironmentType type);

	RESULT SetSkyboxPrograms(std::vector<SkyboxScatterProgram*> pPrograms);
	RESULT SetScreenFadeProgram(OGLProgramScreenFade* pFadeProgram);

	// Environment transition functions
public:
	RESULT HideEnvironment(void *pContext);
	RESULT ShowEnvironment(void *pContext);
	RESULT FadeIn();

	RESULT SwitchToEnvironment(EnvironmentType type);

private:
	point m_ptSceneOffset;
	float m_sceneScale;
	float m_lightIntensity;
	float m_directionalIntensity;

	vector m_vSunDirection = vector(1.0f, 0.75f, -0.5f);
	light *m_pDirectionalSunLight = nullptr;
	light *m_pDirectionalAmbientLight = nullptr;


	// shader programs
	std::vector<SkyboxScatterProgram*> m_skyboxPrograms;
	OGLProgramScreenFade *m_pFadeProgram = nullptr;

private:
	std::shared_ptr<model> m_pCurrentEnvironmentModel = nullptr;
	EnvironmentType m_currentType;

	// environment loading maps
	//TODO: incorporate new environment
	std::map<EnvironmentType, std::wstring> m_environmentFilenames = {
		{ISLAND, L"\\FloatingIsland\\env.obj"},
		{CAVE, L"\\Cave\\cave_no_water_ib.fbx"}
	};

	//populated in LoadAllEnvironments
	std::map<EnvironmentType, std::shared_ptr<model>> m_environmentModels;

	// Environment positioning information (non-island) 
public:
	RESULT SeatUser(point& ptPosition, quaternion& qOrientation, int seatIndex);

private:
	float m_environmentSceneScale = 0.01f;

	float m_tableWidth = 300.0f * m_environmentSceneScale;
	float m_tableLength = 500.0f * m_environmentSceneScale;
	float m_tableHeight = 50.0f * m_environmentSceneScale;

	float m_baseTableAngle = 270.0f * (float)M_PI / 180.0f;
	float m_frontAngle = 135.0f * (float)M_PI / 180.0f;
	float m_middleAngle = 105.0f * (float)M_PI / 180.0f;
	float m_backAngle = 90.0f * (float)M_PI / 180.0f;

	int m_maxSeatingIndex = 6;

};

#endif // ! DREAM_ENVIRONMENT_H_