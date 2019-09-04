#include "CameraController.h"
#include "EnvironmentController.h"

CameraController::CameraController(Controller* pParentController) :
	Controller(pParentController)
{
	// empty
}

CameraController::~CameraController() 
{
	// empty
}

RESULT CameraController::Initialize() {
	RESULT r = R_PASS;

Error:
	return r;
}