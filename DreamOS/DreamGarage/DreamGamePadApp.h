#ifndef DREAM_GAMEPAD_APP_H_
#define DREAM_GAMEPAD_APP_H_

#define GAMEPAD_STRAFESPEED_SCALE 100.0f
#define GAMEPAD_CAMERA_ROTATE_SCALE 10.0f
#define GAMEPAD_CAMERA_UP_SCALE 5000.0f

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGamePadApp.h

#include "DreamApp.h"
#include "Primitives/point.h"
#include "Sense/SenseGamePadController.h"

class DreamGamePadApp : public DreamApp<DreamGamePadApp>, public Subscriber<SenseGamePadEvent> {
	friend class DreamAppManager;

public:
	DreamGamePadApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamGamePadApp();

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamGamePadApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	RESULT Notify(SenseGamePadEvent *pEvent);

private:
	point m_ptLeftStick;
	point m_ptRightStick;

	float m_trigger = 0;
};

#endif // ! DREAM_GAMEPAD_APP_H_
