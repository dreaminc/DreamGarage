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

	RECT rect{ 0 };

	// Need to convert to screen coordinates

	CN(m_pWin64AppParent);
	HWND hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);

	GetClientRect(hwnd, &rect);
	ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&rect.left));

	CB(SetCursorPos(rect.left + x, rect.top + y));

Error:
	return r;
}

RESULT Win64Mouse::GetMousePosition(int& x, int& y) {
	RESULT r = R_PASS;

	RECT clientRect{ 0 };
	HWND hwnd = nullptr;
	POINT ptMousePosition;

	// need to convert to window coordinates

	CN(m_pWin64AppParent);
	
	hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);

	GetClientRect(hwnd, &clientRect);
	ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&clientRect.left));

	CB(GetCursorPos(&ptMousePosition));

	x = ptMousePosition.x - clientRect.left;
	y = ptMousePosition.y - clientRect.top;

Error:
	return r;
}

RESULT Win64Mouse::CenterMousePosition() {
	RESULT r = R_PASS;

	RECT clientRect{ 0 };
	POINT ptCenterScreen;
	HWND hwnd = nullptr;

	CN(m_pWin64AppParent);

	hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);
/*
	RECT wndRect;
	CB(GetWindowRect(hwnd, &wndRect));
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&wndRect, 2);

	int centerX = static_cast<int>(wndRect.left + ((wndRect.right - wndRect.left) / 2.0f));
	int centerY = static_cast<int>(wndRect.top + ((wndRect.bottom - wndRect.top) / 2.0f));
	SenseMouse::SetMousePosition(centerX, centerY);
*/

	CB(GetClientRect(hwnd, &clientRect));

	ptCenterScreen = { (clientRect.left + clientRect.right) / 2, (clientRect.top + clientRect.bottom) / 2 };

	CR(SetMousePosition(ptCenterScreen.x, ptCenterScreen.y));

Error:
	return r;
}

RESULT Win64Mouse::GetCenterPosition(int& x, int& y) {
	RESULT r = R_PASS;
	
	RECT clientRect{ 0 };
	HWND hwnd = nullptr;

	CN(m_pWin64AppParent);

	hwnd = m_pWin64AppParent->GetWindowHandle();
	CN(hwnd);
	/*
	RECT wndRect;
	CB(GetWindowRect(hwnd, &wndRect));
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&wndRect, 2);

	int centerX = static_cast<int>(wndRect.left + ((wndRect.right - wndRect.left) / 2.0f));
	int centerY = static_cast<int>(wndRect.top + ((wndRect.bottom - wndRect.top) / 2.0f));
	SenseMouse::SetMousePosition(centerX, centerY);
	*/

	CB(GetClientRect(hwnd, &clientRect));

	x = (clientRect.left + clientRect.right) / 2;
	y = (clientRect.top + clientRect.bottom) / 2;

Error:
	return r;
}

RESULT Win64Mouse::ShowCursor(bool fShowCursor) {
	RESULT r = R_PASS;

	CB(::ShowCursor(fShowCursor ? TRUE : FALSE));

Error:
	return r;
}

RESULT Win64Mouse::UpdateMousePosition() {
	RESULT r = R_PASS;

	POINT ptMousePosition;
	GetCursorPos(&ptMousePosition);

	UpdateMouseState(SENSE_MOUSE_MOVE, (int)ptMousePosition.x, (int)ptMousePosition.y, 0);

	return r;
}