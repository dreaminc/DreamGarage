#include "Win64Mouse.h"
#include "Windows64App.h"

Win64Mouse::Win64Mouse(Windows64App *pWin64AppParent) :
	SenseMouse(),
	m_pWin64AppParent(pWin64AppParent)
{
	// empty for now
}

RESULT Win64Mouse::CaptureMouse() {
	RESULT r = R_PASS;

	CN(m_pWin64AppParent);

	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);

	SenseMouse::CaptureMouse();
	HWND hwndLast = SetCapture(hwnd);
	CB(ShowCursor(false));

Error:
	return r;
}

RESULT Win64Mouse::ReleaseMouse() {
	RESULT r = R_PASS;

	CN(m_pWin64AppParent);

	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);

	SenseMouse::ReleaseMouse();
	
	CB(ReleaseCapture());
	CB(ShowCursor(true));

Error:
	return r;
}

RESULT Win64Mouse::UpdateMouseState(SenseMouseEventType eventType, int newX, int newY, int state) {
	return SetMouseState(eventType, newX, newY, state);
}

RESULT Win64Mouse::SetMousePosition(int x, int y) {
	RESULT r = R_PASS;

	CN(m_pWin64AppParent);
	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);

	//CR(SenseMouse::SetMousePosition(x, y));
	CB(SetCursorPos(x, y));

Error:
	return r;
}

RESULT Win64Mouse::CenterMousePosition() {
	RESULT r = R_PASS;

	CN(m_pWin64AppParent);

	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);

	RECT wndRect;
	CB(GetWindowRect(hwnd, &wndRect));
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&wndRect, 2);

	int centerX = static_cast<int>(wndRect.left + ((wndRect.right - wndRect.left) / 2.0f));
	int centerY = static_cast<int>(wndRect.top + ((wndRect.bottom - wndRect.top) / 2.0f));
	SenseMouse::SetMousePosition(centerX, centerY);

	CR(SetMousePosition(centerX, centerY));

Error:
	return r;
}

RESULT Win64Mouse::UpdateMousePosition() {
	RESULT r = R_PASS;

	POINT ptMouse;
	GetCursorPos(&ptMouse);

	UpdateMouseState(SENSE_MOUSE_MOVE, (int)ptMouse.x, (int)ptMouse.y, 0);

	return r;
}