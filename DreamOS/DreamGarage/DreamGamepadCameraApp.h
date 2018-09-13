#ifndef DREAM_GAMEPAD_CAMERA_APP_H_
#define DREAM_GAMEPAD_CAMERA_APP_H_

#define GAMEPAD_STRAFESPEED_SCALE 70.0f	
#define GAMEPAD_CAMERA_ROTATE_SCALE 0.65f
#define GAMEPAD_CAMERA_UP_SCALE 20000.0f

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGamepadCameraApp.h

#include "DreamApp.h"
#include "Primitives/point.h"
#include "Sense/SenseGamepadController.h"

#include "Primitives/camera.h"

class DreamGamepadCameraApp : public DreamApp<DreamGamepadCameraApp>, public Subscriber<SenseGamepadEvent> {
	friend class DreamAppManager;

public:
	DreamGamepadCameraApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamGamepadCameraApp();

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamGamepadCameraApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	RESULT Notify(SenseGamepadEvent *pEvent);

	camera *m_pCamera = nullptr;

private:
	// joystick values are 0.0 - 1.0
	// trigger values are 0 - 255
	point2D m_leftStick;
	point2D m_pendLeftStick;

	point2D m_rightStick;
	point2D m_pendRightStick;

	float m_xVelocity = 0.0f;
	float m_yVelocity = 0.0f;
	float m_zVelocity = 0.0f;

	float m_leftTriggerValue = 0.0f;
	float m_pendLeftTriggerValue = 0.0f;
	float m_rightTriggerValue = 0.0f;
	float m_pendRightTriggerValue = 0.0f;

	double m_msTimeLastUpdated = 0.0;

	float m_cameraStrafeSpeed = GAMEPAD_STRAFESPEED_SCALE;
	float m_cameraUpScale = GAMEPAD_CAMERA_UP_SCALE;
	float m_cameraRotateSpeed = GAMEPAD_CAMERA_ROTATE_SCALE;

	bool m_fUpdateLeftStick = false;
	bool m_fUpdateRightStick = false;
	bool m_fUpdateLeftTrigger = false;
	bool m_fUpdateRightTrigger = false;

	bool m_fLockY = false;
};

#endif // ! DREAM_GAMEPAD_APP_H_
