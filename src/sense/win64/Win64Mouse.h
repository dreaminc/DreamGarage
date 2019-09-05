#ifndef WIN64_MOUSE_H_
#define WIN64_MOUSE_H_

#include "core/ehm/EHM.h"

// Dream Sense Win64
// dos/src/sense/win64/Win64Mouse.h

// Sense Windows 64 Mouse Device

#include <windows.h>

#include "Sense/SenseMouse.h"

class Win64Sandbox;

class Win64Mouse : public SenseMouse {
public:
	Win64Mouse(Win64Sandbox *pParentWin64Sandbox);

	virtual RESULT UpdateMouseState(SenseMouseEventType eventType, int newX, int newY, int state) override;

	virtual RESULT CaptureMouse() override;
	virtual RESULT ReleaseMouse() override;
	virtual RESULT SetMousePosition(int x, int y) override;
	virtual RESULT GetMousePosition(int& x, int& y) override;
	virtual RESULT CenterMousePosition() override;
	virtual RESULT GetCenterPosition(int& x, int& y) override;
	virtual RESULT ShowCursor(bool fShowCursor) override;

	RESULT UpdateMousePosition();

private:
	Win64Sandbox *m_pParentWin64Sandbox = nullptr;
};

#endif // ! WIN64_KEYBOARD_H_