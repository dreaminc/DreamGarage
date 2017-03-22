#ifndef DREAM_APP_MANAGER_H_
#define DREAM_APP_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamApp.h
// The base Dream App object

#include "Primitives/Types/UID.h"

#include <queue>
#include <vector>
#include <memory>

// Some priority settings
#define SYSTEM_APP_PRIORITY 0
#define DREAM_APP_PRIORITY 2
#define DEFAULT_APP_PRIORITY DREAM_APP_PRIORITY
#define MAX_APP_PRIORITY 10

using namespace std;

#include "DreamApp.h"

class DreamOS;

class DreamAppManager {
public:
	DreamAppManager(DreamOS *pDreamOS);
	~DreamAppManager();

	RESULT Initialize();

	template<class derivedAppType> 
	RESULT CreateRegisterAndStartApp(void *pContext = nullptr);

private:
	priority_queue<shared_ptr<DreamAppBase>, vector<shared_ptr<DreamAppBase>>, DreamAppBaseCompare> m_appPriorityQueue;
	UID m_uid;
	DreamOS *m_pDreamOS;
};



#endif	// ! DREAM_APP_MANAGER_H_