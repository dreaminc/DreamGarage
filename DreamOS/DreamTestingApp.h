#ifndef DREAM_TEST_APP_H_
#define DREAM_TEST_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamTestingApp.h
// Dream Test App for the purpose of testing out both
// the OS based queue of apps as well as things like 
// system level apps and priority levels 

#include "DreamApp.h"
#include "DreamAppHandle.h"

#include <map>
#include <vector>

class DreamTestingAppHandle : public DreamAppHandle {
public:
//	virtual RESULT;
};

class DreamTestingApp : public DreamApp<DreamTestingApp>, public DreamTestingAppHandle {
	friend class DreamAppManager;

public:
	DreamTestingApp(DreamOS *pDreamOS, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	//RESULT SetVisible(bool fVisible);

	RESULT SetTestingValue(int val);
	int GetTestingValue();

protected:
	static DreamTestingApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	// Member vars go here
	int m_counter = 0;
	int m_testingValue = 0;
};

#endif // ! DREAM_CONTENT_VIEW_H_