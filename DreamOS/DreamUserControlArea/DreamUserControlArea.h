#ifndef DREAM_USER_CONTROL_AREA_H_
#define DREAM_USER_CONTROL_AREA_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"

#include <vector>
#include <map>

class DreamUserApp;

class CEFBrowserManager;

class quad;
 
class DreamUserControlArea : public DreamApp<DreamUserControlArea> {
	friend class DreamAppManager;
	friend class MultiContentTestSuite;

public:
	DreamUserControlArea(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamUserControlArea();

// DreamApp
public:
	virtual RESULT InitializeApp(void *pContext) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamUserControlArea* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

//public:

private:
	std::shared_ptr<DreamUserApp> m_pDreamUserApp;
	std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;

	std::vector<std::shared_ptr<DreamAppBase>> m_pOpenApps;
	std::map<std::shared_ptr<DreamAppBase>, std::shared_ptr<quad>> m_textureUpdateMap;
//	std::vector<DreamApp> *m_openApps;
};

#endif // ! DREAM_USER_CONTROL_AREA_H_