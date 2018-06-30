#include "Win64GamePadController.h"
#include "Windows64App.h"
#include "xinput.h"

Win64GamePadController::Win64GamePadController(Windows64App *pWin64AppParent) :
	SenseGamePadController(),
	m_pWin64AppParent(pWin64AppParent)
{
	// empty for now
}

RESULT Win64GamePadController::UpdateGamePad() {
	RESULT r = R_PASS;
	
	CR(GetGamePadState());

Error:
	return r;
}

RESULT Win64GamePadController::GetGamePadState() {
	RESULT r = R_PASS;

	XINPUT_STATE xInputState;
	memset(&xInputState, 0, sizeof(XINPUT_STATE));

	CRM((RESULT)XInputGetState(0, &xInputState), "Controller not Connected");



Error:
	DEBUG_LINEOUT("No Controller Connected");
	return r;
}