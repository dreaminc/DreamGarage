#ifndef WIN64_MOUSE_H_
#define WIN64_MOUSE_H_

#include <windows.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Win64Mouse.h
// Sense Windows 64 Mouse Device

#include "Sense/SenseMouse.h"

class Windows64App;

class Win64Mouse : public SenseMouse {
public:
	Win64Mouse(Windows64App *pWin64AppParent);

	virtual RESULT UpdateMouseState(SenseMouseEventType eventType, int newX, int newY, int state) override;

	virtual RESULT CaptureMouse() override;
	virtual RESULT ReleaseMouse() override;
	virtual RESULT SetMousePosition(int x, int y) override;
	virtual RESULT GetMousePosition(int& x, int& y) override;
	virtual RESULT CenterMousePosition() override;
	virtual RESULT GetCenterPosition(int& x, int& y) override;
	virtual RESULT ShowCursor(bool show) override;

	RESULT UpdateMousePosition();

private:
	Windows64App *m_pWin64AppParent;
};

#endif // ! WIN64_KEYBOARD_H_