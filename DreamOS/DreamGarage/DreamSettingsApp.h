#ifndef DREAM_SETTINGS_APP_H_
#define DREAM_SETTINGS_APP_H_

#define SCALE_TICK 0.003f;

#include "DreamGarage/DreamFormApp.h"

class DreamUserHandle;
class DreamControlView;

class DreamSettingsApp : public DreamFormApp, 
						public Subscriber<SenseControllerEvent>
{
	friend class DreamAppManager;
	friend class MultiContentTestSuite;

public:
	DreamSettingsApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamSettingsApp();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr);
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr);
	virtual RESULT Update(void *pContext = nullptr);
	virtual RESULT Shutdown(void *pContext = nullptr);

protected:
	static DreamSettingsApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:
	RESULT HandleDreamFormSuccess() override;
private:
	std::string m_strSuccess = "DreamSettingsApp.OnSuccess";

public:
	RESULT Show();
	RESULT Hide();

public:
	std::string GetSuccessString();

public:
	virtual RESULT Notify(SenseControllerEvent *pEvent) override;
	
	// handles settings cancellation
	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;

private:

	std::shared_ptr<UIKeyboard> m_pKeyboard = nullptr;

	float m_scaleTick = SCALE_TICK;

	bool m_fLeftTriggerDown = false;
	bool m_fRightTriggerDown = false;

	bool m_fPendShowFormView = false;

	// initial settings - when the user is logged in and presses back to cancel,
	// the settings should not be saved
private:
	float m_initialHeight;
	float m_initialDepth;
	float m_initialScale;
};

#endif // ! DREAM_SETTINGS_APP_H_
