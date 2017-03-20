#ifndef DREAM_UI_BAR_H_
#define DREAM_UI_BAR_H_

#include "UI/UIBar.h"
#include "Primitives/composite.h"

#include <functional>

class DreamUIBar : public UIBar {

public:
	DreamUIBar(DreamOS *pDreamOS, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat);
	~DreamUIBar();

	RESULT Initialize();
	RESULT Update();

	RESULT HandleTouchStart(void* pContext);
	RESULT HandleTouchMove(void* pContext);
	RESULT HandleTouchEnd(void* pContext);

	// Callback signature
	// RESULT fnEventCallback(struct (opt) pEventInfo, void* pContext)

	RESULT RegisterEvent(InteractionEventType type, std::function<RESULT(void*)> fnCallback);

	std::map<InteractionEventType, std::function<RESULT(void*)>> m_callbacks;

	RESULT Notify(InteractionObjectEvent *event) override;
};


#endif // ! DREAM_UI_BAR_H_