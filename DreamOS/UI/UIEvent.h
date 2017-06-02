#ifndef UI_EVENT_H_
#define UI_EVENT_H_

class VirtualObj;

typedef enum UIEventType {
	UI_EVENT_INTERSECT_BEGAN,
	UI_EVENT_INTERSECT_MOVED,
	UI_EVENT_INTERSECT_ENDED,
	UI_EVENT_INVALID
} UI_EVENT_TYPE;

typedef struct UIEvent {

	UIEventType m_eventType;
	VirtualObj *m_pObj;

	UIEvent(UIEventType eventType, VirtualObj *pObj);
} UI_EVENT;

#endif // ! UI_EVENT_H_