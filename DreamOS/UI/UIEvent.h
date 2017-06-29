#ifndef UI_EVENT_H_
#define UI_EVENT_H_

class VirtualObj;

enum UIEventType {
	UI_EVENT_INTERSECT_BEGAN,
	UI_EVENT_INTERSECT_MOVED,
	UI_EVENT_INTERSECT_ENDED,
	UI_HOVER_BEGIN,
	UI_HOVER_ENDED,
	UI_SELECT_BEGIN,
	UI_SELECT_MOVED,
	UI_SELECT_ENDED,
	UI_MENU,
	UI_EVENT_INVALID
};

struct UIEvent {

	// TODO: this pattern is preferable, but currently doesn't compile
	/*
	enum class type {
		UI_EVENT_INTERSECT_BEGAN,
		UI_EVENT_INTERSECT_MOVED,
		UI_EVENT_INTERSECT_ENDED,
		UI_HOVER_BEGIN,
		UI_HOVER_ENDED,
		UI_SELECT_BEGIN,
		UI_SELECT_MOVED,
		UI_SELECT_ENDED,
		UI_MENU,
		UI_EVENT_INVALID
	};
	//*/

	UIEventType m_eventType;
	VirtualObj *m_pObj;

	UIEvent(UIEventType eventType, VirtualObj *pObj);
};

#endif // ! UI_EVENT_H_