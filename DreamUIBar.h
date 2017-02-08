#ifndef DREAM_UI_BAR_H_
#define DREAM_UI_BAR_H_

#include "UIBar.h"
#include "Primitives/composite.h"

#include <functional>

class DreamUIBar : public UIBar {

	DreamUIBar(composite* c, UIBarFormat info = UIBarFormat());
	~DreamUIBar();

	typedef enum UI_EVENT_TYPE {
		MENU,
		FILE,
		INVALID
	} UIEventType;

	struct UIEventInfo {
		UI_EVENT_TYPE type;

		UIEventInfo() :
			type(INVALID)
		{}
	};

	// Callback signature
	// RESULT fnEventCallback(struct (opt) pEventInfo, void* pContext)

	RESULT RegisterEvent(UI_EVENT_TYPE type, std::function<RESULT(void*)> fnCallback, void* pContext = nullptr);

};


#endif // ! DREAM_UI_BAR_H_