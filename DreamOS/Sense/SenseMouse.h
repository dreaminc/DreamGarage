#ifndef SENSE_MOUSE_H_
#define SENSE_MOUSE_H_

#include <string.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseMouse.h
// Sense Mouse Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

typedef enum SenseMouseEventType {
	SENSE_MOUSE_LEFT_BUTTON_DOWN,
	SENSE_MOUSE_LEFT_BUTTON_UP,
	SENSE_MOUSE_MIDDLE_BUTTON_DOWN,
	SENSE_MOUSE_MIDDLE_BUTTON_UP,
	SENSE_MOUSE_RIGHT_BUTTON_DOWN,
	SENSE_MOUSE_RIGHT_BUTTON_UP,
	SENSE_MOUSE_WHEEL,
	SENSE_MOUSE_MOVE,
	SENSE_MOUSE_LEFT_DRAG_MOVE,
	SENSE_MOUSE_RIGHT_DRAG_MOVE,
	SENSE_MOUSE_INVALID
} SENSE_MOUSE_EVENT_TYPE;

typedef struct SenseMouseEvent : SenseDevice::SenseDeviceEvent {
	SenseMouseEventType EventType;
	int xPos;
	int dx;

	int yPos;
	int dy;

	int state;

	SenseMouseEvent(SenseMouseEventType eventType, int newX, int newY, int oldX, int oldY, int newState) :
		SenseDeviceEvent(),
		EventType(eventType),
		xPos(newX),
		yPos(newY),
		state(newState)
	{
		SenseEventSize = sizeof(SenseMouseEvent);
		dx = newX - oldX;
		dy = newY - oldY;
	}
} SENSE_MOUSE_EVENT;

class SenseMouse : public SenseDevice, public Publisher<SenseMouseEventType, SenseMouseEvent> {
protected:
	//uint8_t m_KeyStates[NUM_SENSE_KEYBOARD_KEYS];

	int m_LeftButtonState;
	int m_MiddleButtonState;
	int m_RightButtonState;

	int m_mouseWheel;

	bool m_fMouseCaptured;

	// Mouse dragging states
	enum class MouseDrag { None, Left, Right };
	MouseDrag m_dragState = MouseDrag::None;
	int m_dragOriginX = 0;
	int m_dragOriginY = 0;

	int m_lastX = 0;
	int m_lastY = 0;

private:
	typedef struct SenseMousePosition {
		int xPos;
		int yPos;
	} SENSE_MOUSE_POSITION;

	SenseMousePosition m_MousePosition;

public:
	SenseMouse();
	~SenseMouse();

	static const char* GetEventTypeName(SenseMouseEventType eventType);

	static const char* GetEventTypeName(SenseMouseEvent event);

	static const char* GetEventTypeName(SenseMouseEvent *pEvent);

	static RESULT PrintEvent(SenseMouseEvent *pEvent);

	virtual RESULT UpdateMouseState(SenseMouseEventType eventType, int newX, int newY, int state) = 0;

	RESULT SetMouseState(SenseMouseEventType eventType, int newX, int newY, int state);

private:
	RESULT GetMouseState(SenseMouseEventType eventType, int &rvalue);

	/*
	SenseMousePostion& GetMousePosition() {
		return m_MousePosition;
	}
	*/

protected:
	// x,y are in window screen coordinates!
	virtual RESULT SetMousePosition(int x, int y);
	virtual RESULT ShowCursor(bool show);

public:
	virtual RESULT GetMousePosition(int& x, int& y);

	virtual RESULT CaptureMouse();
	virtual RESULT ReleaseMouse();

	// The SenseMouse interface
	virtual RESULT CenterMousePosition() = 0;
	virtual RESULT GetCenterPosition(int& x, int& y) = 0;
	virtual RESULT UpdateMousePosition() = 0;
	//virtual RESULT SetMousePosition(int x, int y) = 0;

};

#endif // ! SENSE_MOUSE_H_