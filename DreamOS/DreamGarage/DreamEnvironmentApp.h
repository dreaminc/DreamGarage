#ifndef DREAM_ENVIRONMENT_H_
#define DREAM_ENVIRONMENT_H_

#include "./RESULT/EHM.h"
#include "DreamApp.h"
#include "Primitives/point.h"
#include "Primitives/color.h"
#include "Primitives/FogParams.h"

#include <map>

class DreamOS;
class DreamUserApp;
class quad;
class model;
class light;
class SkyboxScatterProgram;
class FogProgram;
//TODO: move to proxy?
class OGLProgramScreenFade;
class user;
class ObjectStoreNode;

#define SCENE_SCALE 0.025f

#define TABLE_LENGTH (187.5f * SCENE_SCALE)

#define SHARED_SCREEN_SCALE 0.55f
#define SHARED_SCREEN_POSITION point(0.5f * TABLE_LENGTH, 0.125f, 0.0f)

namespace environment {
	typedef enum type {
		LOBBY = 0,
		CAVE = 1,
		CANYON = 2,
		HOUSE = 3,
		ISLAND,
		INVALID
	} ENVIRONMENT_TYPE;
};

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
	RESULT PositionEnvironment(environment::type type, std::shared_ptr<model> pModel);
	RESULT LoadAllEnvironments();
	RESULT SetCurrentEnvironment(environment::type type);

	RESULT SetSkyboxPrograms(std::vector<SkyboxScatterProgram*> skyboxPrograms);
	RESULT SetScreenFadeProgram(OGLProgramScreenFade* pFadeProgram);

	RESULT SetFogPrograms(std::vector<FogProgram*> fogPrograms);

	// Environment transition functions
public:
	RESULT HideEnvironment(void *pContext);
	RESULT ShowEnvironment(void *pContext);
	RESULT FadeIn();
	RESULT FadeOut(std::function<RESULT(void*)> fnFadeOutCallback = nullptr);

	RESULT SendOnFadeInMessage(void *pContext);

	RESULT SwitchToEnvironment(environment::type type);
	RESULT GetSharedScreenPlacement(point& ptPosition, quaternion& qOrientation, float& scale);
	RESULT GetDefaultCameraPlacement(point& ptPosition, quaternion& qOrientation);

	ObjectStoreNode *GetSceneGraphNode() {
		return m_pSceneGraph;
	}

private:
	point m_ptSceneOffset;
	float m_sceneScale;
	float m_lightIntensity;
	float m_directionalIntensity;

	vector m_vSunDirection = vector(1.0f, 0.25f, -0.1f);
	light *m_pDirectionalSunLight = nullptr;
	light *m_pDirectionalAmbientLight = nullptr;

	// shader programs
	std::vector<SkyboxScatterProgram*> m_skyboxPrograms;
	OGLProgramScreenFade *m_pFadeProgram = nullptr;

	std::vector<FogProgram*> m_fogPrograms;

private:
	std::shared_ptr<model> m_pCurrentEnvironmentModel = nullptr;
	environment::type m_currentType;

	// environment loading maps
	//TODO: incorporate new environment
	std::map<environment::type, std::wstring> m_environmentFilenames = {
		// legacy environment, potentially could be removed completely
		// do not load during release startup
	//	{environment::ISLAND, L"\\FloatingIsland\\env.obj"},
		{environment::CAVE, L"\\model\\environment\\1\\environment.fbx"},
		//{environment::CANYON, L"\\model\\environment\\2\\environment.fbx"},
		//{environment::HOUSE, L"\\model\\environment\\3\\environment.fbx"}
	};
	
	std::map<environment::type, FogParams> m_environmentFogParams = {
		{ environment::CAVE, FogParams(50.0f, 300.0f, 0.05f, color(161.0f / 255.0f, 197.0f / 255.0f, 202.0f / 255.0f, 1.0f))},
		{ environment::CANYON, FogParams(900.0f, 1150.0f, 0.05f, color(202.0f / 255.0f, 190.0f / 255.0f, 161.0f / 255.0f, 1.0f))},	// 450 is ~the last leg of the bend, but probably need a better distance solution
		{ environment::HOUSE, FogParams(50.0f, 300.0f, 0.05f, color(161.0f / 255.0f, 197.0f / 255.0f, 202.0f / 255.0f, 1.0f))}
	};

	std::map<environment::type, vector> m_environmentSunDirection = {
		{ environment::CAVE, vector(1.0f, 0.25f, -0.1f) },	// cave
		{ environment::CANYON, vector(-0.4f, 0.5f, 1.0f) },	// canyon
		{ environment::HOUSE, vector(0.5f, 0.13f, 0.1f) }	// House front right
	};

	//populated in LoadAllEnvironments
	std::map<environment::type, std::shared_ptr<model>> m_environmentModels;

	// Environment positioning information (non-island) 
public:
	RESULT GetEnvironmentSeatingPositionAndOrientation(point& ptPosition, quaternion& qOrientation, int seatIndex);
	quaternion GetUIOffsetOrientation(int seatIndex);

private:
	float m_environmentSceneScale = SCENE_SCALE;

	float m_tableWidth = 112.5f * m_environmentSceneScale;
	float m_tableLength = TABLE_LENGTH;
	float m_tableHeight = 25.0f * m_environmentSceneScale;

	float m_baseTableAngle = 270.0f * (float)M_PI / 180.0f;
	float m_frontAngle = 135.0f * (float)M_PI / 180.0f;
	float m_middleAngle = 105.0f * (float)M_PI / 180.0f;
	float m_backAngle = 90.0f * (float)M_PI / 180.0f;

	int m_maxSeatingIndex = 6;

	std::string m_strOnFadeInString = "DreamEnvironmentApp.OnFadeIn";

	point m_ptSharedScreen = SHARED_SCREEN_POSITION;
	float m_sharedScreenScale = SHARED_SCREEN_SCALE;

private:
	ObjectStoreNode *m_pSceneGraph = nullptr;
};

#endif // ! DREAM_ENVIRONMENT_H_