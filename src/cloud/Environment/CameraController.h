#ifndef CAMERA_CONTROLLER_H_
#define CAMERA_CONTROLLER_H_

#include "core/ehm/EHM.h"

// Dream Cloud Camera Controller
// TODO: Move this somewhere else (specific to an app?)
// Add some functionality here 

#include "core/types/Controller.h"

class CameraController : public Controller {
	friend class EnvironmentController;
public:
	CameraController(Controller* pParentController);
	~CameraController();

	virtual RESULT Initialize() override;
};

#endif // ! CAMERA_CONTROLLER_H_