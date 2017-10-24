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
class VirtualObj;

// determines the app that handles the controller "menu up" event
enum class ActiveAppType {
	MENU,		// default state
	KB_MENU,	// kb presents when "share website" is pressed in the menu
	CONTROL,	// control presents when "enter" is pressed on the kb (browser) 
	KB_CONTROL,	// kb (control) presents when there is text focus
	NONE,
	INVALID
};

class DreamUserHandle : public DreamAppHandle {
public:
	//TODO: this is unsafe, since the mallets can be used later, 
	// potentially, this function should return a handle to a mallet
	UIMallet *RequestMallet(HAND_TYPE type);

	RESULT SendPresentApp(ActiveAppType type);

private:
	virtual UIMallet *GetMallet(HAND_TYPE type) = 0;
	virtual RESULT PresentApp(ActiveAppType type) = 0;

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

protected:
	static DreamUserApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:
	virtual RESULT Notify(InteractionObjectEvent *mEvent) override;

	RESULT SetHand(hand* pHand);

	virtual UIMallet *GetMallet(HAND_TYPE type) override;
	virtual RESULT PresentApp(ActiveAppType type) override;

protected:
	RESULT UpdateCompositeWithCameraLook(float depth, float yPos);
	RESULT UpdateCompositeWithHands(float yPos);

private:
	// Member vars go here
	//user *m_pUserModel = nullptr;
	std::shared_ptr<volume> m_pVolume = nullptr;
	std::shared_ptr<DimRay> m_pOrientationRay = nullptr;
	
	hand* m_pLeftHand = nullptr;
	hand* m_pRightHand = nullptr;

	UIMallet* m_pLeftMallet = nullptr;
	UIMallet* m_pRightMallet = nullptr;

	// the user app maintains an active app state to send events to the right app 
	ActiveAppType m_activeState = ActiveAppType::NONE; 

	// apps position themselves with this when they are presented
	VirtualObj *m_pAppBasis;
};

#endif // ! DREAM_USER_APP_H_