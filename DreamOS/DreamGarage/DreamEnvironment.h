#ifndef DREAM_ENVIRONMENT_H_
#define DREAM_ENVIRONMENT_H_

#include "./RESULT/EHM.h"
#include "DreamApp.h"
#include "Primitives/point.h"

class DreamOS;

class DreamEnvironment : public DreamApp<DreamEnvironment> {
	friend class DreamAppManager;

	// DreamApp
public:
	DreamEnvironment(DreamOS *pDreamOS, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamEnvironment* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	point m_ptSceneOffset;
	float m_sceneScale;
	float m_lightIntensity;

};

#endif // ! DREAM_ENVIRONMENT_H_