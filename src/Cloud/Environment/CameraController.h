#ifndef CAMERA_CONTROLLER_H_
#define CAMERA_CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Cloud/Controller.h"

class CameraController : public Controller {
	friend class EnvironmentController;
public:
	CameraController(Controller* pParentController);
	~CameraController();

	virtual RESULT Initialize() override;
};

#endif // ! CAMERA_CONTROLLER_H_