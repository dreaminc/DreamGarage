#ifndef DREAM_ENVIRONMENT_H_
#define DREAM_ENVIRONMENT_H_

#include "./RESULT/EHM.h"
#include "DreamApp.h"
#include "Primitives/point.h"

class DreamOS;
class quad;
class light;
class SkyboxScatterProgram;

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
	RESULT SetSkyboxPrograms(std::vector<SkyboxScatterProgram*> pPrograms);

private:
	point m_ptSceneOffset;
	float m_sceneScale;
	float m_lightIntensity;
	float m_directionalIntensity;

	vector m_vSunDirection = vector(1.0f, 0.75f, -0.5f);
	light *m_pDirectionalSunLight = nullptr;
	light *m_pDirectionalAmbientLight = nullptr;

	std::vector<SkyboxScatterProgram*> m_skyboxPrograms;
};

#endif // ! DREAM_ENVIRONMENT_H_