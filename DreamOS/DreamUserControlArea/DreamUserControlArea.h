#ifndef DREAM_USER_CONTROL_AREA_H_
#define DREAM_USER_CONTROL_AREA_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"

class DreamUserApp;

class WebBrowserManager;
 
class DreamUserControlArea : public DreamApp<DreamUserControlArea> {
	friend class DreamAppManager;

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
	std::shared_ptr<WebBrowserManager> m_pWebBrowserManager;

//	std::vector<DreamApp> *m_openApps;
}

#endif // ! DREAM_USER_CONTROL_AREA_H_