#ifndef DREAM_UI_BAR_H_
#define DREAM_UI_BAR_H_

#include "UIBar.h"
#include "Primitives/composite.h"

#include <functional>

class DreamUIBar : public UIBar {

public:
	DreamUIBar(composite* pComposite, UIMenuItem::IconFormat iconFormat, UIMenuItem::LabelFormat labelFormat, UIBarFormat barFormat);
	~DreamUIBar();

	typedef enum UI_MENU_ITEM_EVENT {
		FORWARD,
		FILE,
		INVALID
	} UIMenuItemEvent;

	struct UIEventInfo {
		UIMenuItemEvent type;

		UIEventInfo() :
			type(INVALID)
		{}
	};

	RESULT Initialize();

	// Callback signature
	// RESULT fnEventCallback(struct (opt) pEventInfo, void* pContext)

	RESULT RegisterEvent(UIMenuItemEvent type, std::function<RESULT(void*)> fnCallback, void* pContext = nullptr);
};


#endif // ! DREAM_UI_BAR_H_