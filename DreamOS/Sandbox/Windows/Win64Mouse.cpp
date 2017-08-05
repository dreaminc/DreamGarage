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

	if (m_fMouseCaptured == false) {
		SenseMouse::CaptureMouse();
		HWND hwndLast = SetCapture(hwnd);
		CB(ShowCursor(false));
	}

Error:
	return r;
}

RESULT Win64Mouse::ReleaseMouse() {
	RESULT r = R_PASS;

	CN(m_pWin64AppParent);

	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);

	if (m_fMouseCaptured) {
		SenseMouse::ReleaseMouse();
		CB(ReleaseCapture());
		CB(ShowCursor(true));
	}

Error:
	return r;
}

RESULT Win64Mouse::UpdateMouseState(SenseMouseEventType eventType, int newX, int newY, int state) {
	return SetMouseState(eventType, newX, newY, state);
}

RESULT Win64Mouse::SetMousePosition(int x, int y) {
	RESULT r = R_PASS;

	// need to convert to screen coordinates

	CN(m_pWin64AppParent);
	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);

	RECT rect{ 0 };
	GetClientRect(hwnd, &rect);
	ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&rect.left));

	CB(SetCursorPos(rect.left + x, rect.top + y));

Error:
	return r;
}

RESULT Win64Mouse::GetMousePosition(int& x, int& y) {
	RESULT r = R_PASS;

	// need to convert to window coordinates

	CN(m_pWin64AppParent);
	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);

	RECT rect{ 0 };
	GetClientRect(hwnd, &rect);
	ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&rect.left));

	POINT pos;
	CB(GetCursorPos(&pos));

	x = pos.x - rect.left;
	y = pos.y - rect.top;

Error:
	return r;
}

RESULT Win64Mouse::CenterMousePosition() {
	RESULT r = R_PASS;

	CN(m_pWin64AppParent);

	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);
/*
	RECT wndRect;
	CB(GetWindowRect(hwnd, &wndRect));
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&wndRect, 2);

	int centerX = static_cast<int>(wndRect.left + ((wndRect.right - wndRect.left) / 2.0f));
	int centerY = static_cast<int>(wndRect.top + ((wndRect.bottom - wndRect.top) / 2.0f));
	SenseMouse::SetMousePosition(centerX, centerY);
*/

	RECT rect{ 0 };
	CB(GetClientRect(hwnd, &rect));

	POINT centerScreen = { (rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2 };

	CR(SetMousePosition(centerScreen.x, centerScreen.y));

Error:
	return r;
}

RESULT Win64Mouse::GetCenterPosition(int& x, int& y) {
	RESULT r = R_PASS;

	CN(m_pWin64AppParent);

	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);
	/*
	RECT wndRect;
	CB(GetWindowRect(hwnd, &wndRect));
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&wndRect, 2);

	int centerX = static_cast<int>(wndRect.left + ((wndRect.right - wndRect.left) / 2.0f));
	int centerY = static_cast<int>(wndRect.top + ((wndRect.bottom - wndRect.top) / 2.0f));
	SenseMouse::SetMousePosition(centerX, centerY);
	*/

	RECT rect{ 0 };
	CB(GetClientRect(hwnd, &rect));

	x = (rect.left + rect.right) / 2;
	y = (rect.top + rect.bottom) / 2;

Error:
	return r;
}

RESULT Win64Mouse::ShowCursor(bool show) {
	RESULT r = R_PASS;

	CB(::ShowCursor(show ? TRUE : FALSE));

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