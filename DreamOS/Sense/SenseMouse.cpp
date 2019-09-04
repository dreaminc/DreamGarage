#include "SenseMouse.h"
#include "DreamLogger/DreamLogger.h"

SenseMouse::SenseMouse() :
		m_LeftButtonState(0),
		m_MiddleButtonState(0),
		m_RightButtonState(0),
		m_fMouseCaptured(false) {
	SetMousePosition(0, 0);

	for (int i = 0; i < SENSE_MOUSE_INVALID; i++) {
		RegisterEvent((SenseMouseEventType)(i));
	}
}


SenseMouse::~SenseMouse() {
	// empty stub
}

const char* SenseMouse::GetEventTypeName(SenseMouseEventType eventType) {
	switch (eventType) {
	case SENSE_MOUSE_LEFT_BUTTON_UP: return "left button up";
	case SENSE_MOUSE_LEFT_BUTTON_DOWN: return "left button down";
	case SENSE_MOUSE_MIDDLE_BUTTON_UP: return "middle button up";
	case SENSE_MOUSE_MIDDLE_BUTTON_DOWN: return "middle button down";
	case SENSE_MOUSE_RIGHT_BUTTON_UP: return "right button up";
	case SENSE_MOUSE_RIGHT_BUTTON_DOWN: return "right button down";
	case SENSE_MOUSE_WHEEL: return "wheel";
	case SENSE_MOUSE_MOVE: return "position";

	default:
	case SENSE_MOUSE_INVALID: return "invalid";
	}
}

const char* SenseMouse::GetEventTypeName(SenseMouseEvent event) {
	return GetEventTypeName(event.EventType);
}

const char* SenseMouse::GetEventTypeName(SenseMouseEvent *pEvent) {
	return GetEventTypeName(pEvent->EventType);
}

RESULT SenseMouse::PrintEvent(SenseMouseEvent *pEvent) {
	DEBUG_LINEOUT("Sense Mouse Event %s x:%d y:%d dx:%d dy:%d state:%d",
		GetEventTypeName(pEvent), pEvent->xPos, pEvent->yPos, pEvent->dx, pEvent->dy, pEvent->state);

	return R_PASS;
}

SenseMouse::MouseDragState GetMouseDragButton(SenseMouseEventType eventType) {
	switch (eventType) {
		case SENSE_MOUSE_LEFT_BUTTON_UP:
		case SENSE_MOUSE_LEFT_BUTTON_DOWN: {
			return SenseMouse::MouseDragState::LEFT;
		} break;

		case SENSE_MOUSE_MIDDLE_BUTTON_UP:
		case SENSE_MOUSE_MIDDLE_BUTTON_DOWN: {
			return SenseMouse::MouseDragState::MIDDLE;
		} break;

		case SENSE_MOUSE_RIGHT_BUTTON_UP: 
		case SENSE_MOUSE_RIGHT_BUTTON_DOWN: {
			return SenseMouse::MouseDragState::RIGHT;
		} break;
	}

	return SenseMouse::MouseDragState::NONE;
}

SenseMouseEventType GetMouseDragEvent(SenseMouse::MouseDragState dragState) {
	switch (dragState) {
	case SenseMouse::MouseDragState::LEFT: return SENSE_MOUSE_LEFT_DRAG_MOVE; break;
	case SenseMouse::MouseDragState::MIDDLE: return SENSE_MOUSE_MIDDLE_DRAG_MOVE; break;
	case SenseMouse::MouseDragState::RIGHT: return SENSE_MOUSE_RIGHT_DRAG_MOVE; break;
	}

	return SENSE_MOUSE_INVALID;
}

RESULT SenseMouse::SetMouseState(SenseMouseEventType eventType, int newX, int newY, int state) {
	RESULT r = R_PASS;

	SenseMouseEvent mEvent(eventType, newX, newY, m_lastX, m_lastY, state);
	mEvent.dx = (newX - m_lastX);
	mEvent.dy = (newY - m_lastY);
	m_MousePosition = { newX, newY };

	switch (eventType) {
	case SENSE_MOUSE_LEFT_BUTTON_UP:
	case SENSE_MOUSE_MIDDLE_BUTTON_UP:
	case SENSE_MOUSE_RIGHT_BUTTON_UP: {
		m_dragState = m_dragState & (~GetMouseDragButton(eventType));

		CR(NotifySubscribers(eventType, &mEvent));
	} break;

	case SENSE_MOUSE_LEFT_BUTTON_DOWN: 
	case SENSE_MOUSE_MIDDLE_BUTTON_DOWN:
	case SENSE_MOUSE_RIGHT_BUTTON_DOWN: {
		if (m_dragState == MouseDragState::NONE) {
			m_dragOriginX = newX;
			m_dragOriginY = newY;
		}

		m_dragState = m_dragState | GetMouseDragButton(eventType);

		CR(NotifySubscribers(eventType, &mEvent));
	} break;

	case SENSE_MOUSE_WHEEL: {
		CR(NotifySubscribers(eventType, &mEvent));
	} break;

	case SENSE_MOUSE_MOVE: {
		CR(NotifySubscribers(eventType, &mEvent));

		if (m_dragState != MouseDragState::NONE) {
			SenseMouseEvent mDragEvent(eventType, newX, newY, m_lastX, m_lastY, state);

			int xCenter = 0, yCenter = 0;
			GetCenterPosition(xCenter, yCenter);

			int xPos = 0, yPos = 0;
			GetMousePosition(xPos, yPos);

			mEvent.dx = (xPos - xCenter) * 1;
			mEvent.dy = (yPos - yCenter) * 1;

			for (int i = 1; i <= (int)(SenseMouse::MouseDragState::RIGHT); i <<= 1) {
				SenseMouse::MouseDragState mouseState = (SenseMouse::MouseDragState)(i);

				if ((m_dragState | mouseState) != MouseDragState::NONE) {
					SenseMouseEventType dragEventType = GetMouseDragEvent(mouseState);
					mDragEvent.EventType = dragEventType;

					CR(NotifySubscribers(dragEventType, &mDragEvent));
				}
			}
		}
	} break;

	case SENSE_MOUSE_LEFT_DRAG_MOVE:
	case SENSE_MOUSE_RIGHT_DRAG_MOVE:
	default: {
		// TODO: ?
		CR(NotifySubscribers(eventType, &mEvent));
	} break;
	}

	/*
	// TODO: Fix dragging
	if (m_dragState == MouseDrag::NONE && (eventType == SENSE_MOUSE_LEFT_BUTTON_DOWN || eventType == SENSE_MOUSE_RIGHT_BUTTON_DOWN)) {
		m_dragState = (eventType == SENSE_MOUSE_LEFT_BUTTON_DOWN) ? MouseDrag::LEFT_BUTTON : MouseDrag::RIGHT_BUTTON;

		m_dragOriginX = newX;
		m_dragOriginY = newY;
	}

	if ((m_dragState == MouseDrag::LEFT_BUTTON && eventType == SENSE_MOUSE_LEFT_BUTTON_UP) ||
		(m_dragState == MouseDrag::RIGHT_BUTTON && eventType == SENSE_MOUSE_RIGHT_BUTTON_UP) ){
		m_dragState = MouseDrag::NONE;
	}

	if (m_dragState != MouseDrag::NONE && eventType == SENSE_MOUSE_MOVE) {
		if (m_dragState == MouseDrag::LEFT_BUTTON) {
			eventType = SENSE_MOUSE_LEFT_DRAG_MOVE;
			mEvent.EventType = SENSE_MOUSE_LEFT_DRAG_MOVE;
		}
		else {
			eventType = SENSE_MOUSE_RIGHT_DRAG_MOVE;
			mEvent.EventType = SENSE_MOUSE_RIGHT_DRAG_MOVE;
		}

		int xCenter = 0, yCenter = 0;
		GetCenterPosition(xCenter, yCenter);

		int xPos = 0, yPos = 0;
		GetMousePosition(xPos, yPos);

		mEvent.dx = (xPos - xCenter) * 1;
		mEvent.dy = (yPos - yCenter) * 1;
	}

	if ((eventType == SENSE_MOUSE_LEFT_DRAG_MOVE || eventType == SENSE_MOUSE_RIGHT_DRAG_MOVE) && (mEvent.dx != 0 || mEvent.dy != 0)) {
		CR(NotifySubscribers(eventType, &mEvent));
	}
	else 
	*/

Error:
	//m_lastX = newX;
	//m_lastY = newY;

	return r;
}

RESULT SenseMouse::GetMouseState(SenseMouseEventType eventType, int &rvalue) {
	RESULT r = R_PASS;
/*
	CB((eventType <= SENSE_MOUSE_RIGHT_BUTTON));

	switch (eventType) {
	case SENSE_MOUSE_LEFT_BUTTON: rvalue = m_LeftButtonState; break;
	case SENSE_MOUSE_MIDDLE_BUTTON: rvalue = m_MiddleButtonState; break;
	case SENSE_MOUSE_RIGHT_BUTTON: rvalue = m_RightButtonState; break;
	case SENSE_MOUSE_WHEEL: rvalue = m_mouseWheel; break;
	}

Error:*/
	return r;
}

RESULT SenseMouse::SetMousePosition(int x, int y) {
	m_MousePosition.xPos = x;
	m_MousePosition.yPos = y;

	return R_PASS;
}

RESULT SenseMouse::GetMousePosition(int& x, int& y) {
	x = m_MousePosition.xPos;
	y = m_MousePosition.yPos;

	return R_PASS;
}

RESULT SenseMouse::ShowCursor(bool show) {
	// OS dependent, not implemented here.
	return R_PASS;
}

RESULT SenseMouse::CaptureMouse() {
	m_fMouseCaptured = true;
	return R_PASS;
}

RESULT SenseMouse::ReleaseMouse() {
	m_fMouseCaptured = false;
	return R_PASS;
}
