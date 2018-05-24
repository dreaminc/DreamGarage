#ifndef DREAM_ENVIRONMENT_H_
#define DREAM_ENVIRONMENT_H_

#include "./RESULT/EHM.h"
#include "DreamApp.h"

class DreamOS;

class DreamEnvironment : public DreamApp<DreamEnvironment> {
	friend class DreamAppManager;

public:

	DreamEnvironment(DreamOS *pDreamOS, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamEnvironment* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

};

#endif // ! DREAM_ENVIRONMENT_H_