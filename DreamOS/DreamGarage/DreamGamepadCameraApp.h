#ifndef DREAM_GAMEPAD_CAMERA_APP_H_
#define DREAM_GAMEPAD_CAMERA_APP_H_

#define GAMEPAD_MOVE_SCALE 23914.0f;
#define GAMEPAD_UP_SPEED_SCALE 25000000.0f;		// joystick values are 0-1, triggers are 0-255
#define GAMEPAD_CAMERA_ROTATE_SCALE 1.41178f;

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGamepadCameraApp.h

#include "DreamApp.h"
#include "Primitives/point.h"
#include "Primitives/point2D.h"
#include "Sense/SenseGamepadController.h"
#include "Sense/SenseController.h"

class camera;
class AirResistanceGenerator;

class DreamGamepadCameraApp : public DreamApp<DreamGamepadCameraApp>, 
							  public Subscriber<SenseGamepadEvent>,
							  public Subscriber<SenseControllerEvent> {
	friend class DreamAppManager;

public:
	enum class CameraControlType {
		GAMEPAD,
		SENSECONTROLLER,
		INVALID
	};

	DreamGamepadCameraApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamGamepadCameraApp();

	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	RESULT SetCamera(camera *pCamera, CameraControlType controlType = CameraControlType::GAMEPAD);
	CameraControlType GetCameraControlType();

	RESULT UnregisterFromEvents();

protected:
	static DreamGamepadCameraApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	virtual RESULT Notify(SenseGamepadEvent *pEvent) override;
	virtual RESULT Notify(SenseControllerEvent *pEvent) override;

	camera *m_pCamera = nullptr;

private:
	// joystick values are 0.0 - 1.0
	// trigger values are 0 - 255
	point2D m_ptLeftStick;
	point2D m_ptPendLeftStick;

	point2D m_ptRightStick;
	point2D m_ptPendRightStick;
	
	float m_leftTriggerValue = 0.0f;
	float m_pendLeftTriggerValue = 0.0f;
	float m_rightTriggerValue = 0.0f;
	float m_pendRightTriggerValue = 0.0f;
	
	float m_lookXVelocity = 0.0f;
	float m_lookYVelocity = 0.0f;

	double m_msTimeLastUpdated = 0.0;
	
	float m_cameraMoveSpeedScale = GAMEPAD_MOVE_SCALE;
	float m_cameraUpSpeedScale = GAMEPAD_UP_SPEED_SCALE;
	float m_cameraRotateSpeed = GAMEPAD_CAMERA_ROTATE_SCALE;
	
	bool m_fUpdateLeftStick = false;
	bool m_fUpdateRightStick = false;
	bool m_fUpdateLeftTrigger = false;
	bool m_fUpdateRightTrigger = false;
	
	bool m_fLockY = false;
	
	CameraControlType m_controlType = CameraControlType::INVALID;
	std::list<ForceGenerator*> m_pForceGenerators;
};

#endif // ! DREAM_GAMEPAD_APP_H_
