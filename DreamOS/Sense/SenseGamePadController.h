#ifndef SENSE_GAMEPAD_CONTROLLER_H_
#define SENSE_GAMEPAD_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseMouse.h
// Sense Mouse Device

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

typedef enum SenseGamePadEventType {
	SENSE_MOUSE_LEFT_BUTTON_DOWN,
	SENSE_MOUSE_LEFT_BUTTON_UP,
	SENSE_MOUSE_MIDDLE_BUTTON_DOWN,
	SENSE_MOUSE_MIDDLE_BUTTON_UP,
	SENSE_MOUSE_RIGHT_BUTTON_DOWN,
	SENSE_MOUSE_RIGHT_BUTTON_UP,
	SENSE_MOUSE_WHEEL,
	SENSE_MOUSE_MOVE,
	SENSE_MOUSE_LEFT_DRAG_MOVE,
	SENSE_MOUSE_MIDDLE_DRAG_MOVE,
	SENSE_MOUSE_RIGHT_DRAG_MOVE,
	SENSE_MOUSE_INVALID
} SENSE_GAMEPAD_EVENT_TYPE;

typedef struct SenseGamePadEvent : SenseDevice::SenseDeviceEvent {
	SenseGamePadEventType EventType;

	SenseGamePadEvent(SenseGamePadEventType eventType, int newX, int newY, int oldX, int oldY, int newState) :
		SenseDeviceEvent(),
		EventType(eventType),
		xPos(newX),
		yPos(newY),
		state(newState)
	{
		SenseEventSize = sizeof(SenseGamePadEvent);
		dx = newX - oldX;
		dy = newY - oldY;
	}
} SENSE_MOUSE_EVENT;

class SenseGamePadController : public SenseDevice, public Publisher<SenseGamePadEventType, SenseGamePadEvent> {

protected:
	//uint8_t m_KeyStates[NUM_SENSE_KEYBOARD_KEYS];

	int m_LeftButtonState;
	int m_MiddleButtonState;
	int m_RightButtonState;

	int m_mouseWheel;

	bool m_fMouseCaptured;

	MouseDragState m_dragState = MouseDragState::NONE;
	int m_dragOriginX = 0;
	int m_dragOriginY = 0;

	int m_lastX = 0;
	int m_lastY = 0;

public:
	SenseGamePadController();
	~SenseGamePadController();

	RESULT UpdateGamePadState();

	static const char* GetEventTypeName(SenseGamePadEventType eventType);
	static const char* GetEventTypeName(SenseGamePadEvent event);
	static const char* GetEventTypeName(SenseGamePadEvent *pEvent);

	static RESULT PrintEvent(SenseGamePadEvent *pEvent);

private:
	RESULT GetGamePadState(SenseMouseEventType eventType, int &rvalue);
}

#endif // ! SENSE_GAMEPAD_CONTROLLER_H_
