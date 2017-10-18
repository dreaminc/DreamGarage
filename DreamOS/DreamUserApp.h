#ifndef DREAM_USER_APP_H_
#define DREAM_USER_APP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamUserApp.h
// The Dream User app is the app to contain all of the 
// relevant user information (and assets)

#include "Primitives/Subscriber.h"
#include "DreamApp.h"
#include "DreamAppHandle.h"
#include "Primitives/HandType.h"

#include <map>
#include <vector>

struct InteractionObjectEvent;

class volume;
class hand;
class UIMallet;
class DimRay;

class DreamUserHandle : public DreamAppHandle {
public:
	//TODO: this is unsafe, since the mallets can be used later, 
	// potentially, this function should return a handle to a mallet
	UIMallet *RequestMallet(HAND_TYPE type);

private:
	virtual UIMallet *GetMallet(HAND_TYPE type) = 0;

};

class DreamUserApp : public DreamApp<DreamUserApp>, public DreamUserHandle, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;

public:
	DreamUserApp(DreamOS *pDreamOS, void *pContext = nullptr);

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual DreamAppHandle *GetAppHandle() override;
public:
	// Member funcs 

public:
	virtual RESULT Notify(InteractionObjectEvent *mEvent) override;

	RESULT SetHand(hand* pHand);

	virtual UIMallet *GetMallet(HAND_TYPE type) override;

protected:
	static DreamUserApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	// Member vars go here
	//user *m_pUserModel = nullptr;
	std::shared_ptr<volume> m_pVolume = nullptr;
	std::shared_ptr<DimRay> m_pOrientationRay = nullptr;
	
	hand* m_pLeftHand = nullptr;
	hand* m_pRightHand = nullptr;

	UIMallet* m_pLeftMallet = nullptr;
	UIMallet* m_pRightMallet = nullptr;
};

#endif // ! DREAM_USER_APP_H_