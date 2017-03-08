#ifndef DREAM_UI_BAR_H_
#define DREAM_UI_BAR_H_

#include "UI/UIBar.h"
#include "Primitives/composite.h"

#include <functional>

class DreamUIBar : public UIBar {

public:
	DreamUIBar(DreamOS *pDreamOS, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat);
	~DreamUIBar();

	//TODO: may be moved depending on implementation of registration architecture
	typedef enum class UI_MENU_ITEM_EVENT {
		FORWARD,
		FILE,
		INVALID
	} UIMenuItemEvent;

	struct UIEventInfo {
		UIMenuItemEvent type;

		UIEventInfo() :
			type(UIMenuItemEvent::INVALID)
		{}
	};

	RESULT Initialize();
	RESULT Update();

	// Callback signature
	// RESULT fnEventCallback(struct (opt) pEventInfo, void* pContext)

	RESULT RegisterEvent(UIMenuItemEvent type, std::function<RESULT(void*)> fnCallback, void* pContext = nullptr);
};


#endif // ! DREAM_UI_BAR_H_