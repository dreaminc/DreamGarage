#ifndef DREAM_UI_BAR_H_
#define DREAM_UI_BAR_H_

#include "UI/UIBar.h"
#include "Primitives/composite.h"

#include <functional>

#include "Cloud/Menu/MenuController.h"

class DreamUIBar : public UIBar, public MenuController::observer {

public:
	DreamUIBar(DreamOS *pDreamOS, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat);
	~DreamUIBar();

	RESULT Initialize();
	RESULT Update();

	RESULT HandleTouchStart(void* pContext);
	RESULT HandleTouchMove(void* pContext);
	RESULT HandleTouchEnd(void* pContext);

	// TODO: these functions are related to SenseController instead of the
	// Interaction Engine

	RESULT HandleMenuUp();
	RESULT HandleTriggerUp();

	// Callback signature
	// RESULT fnEventCallback(struct (opt) pEventInfo, void* pContext)

	RESULT RegisterEvent(InteractionEventType type, std::function<RESULT(void*)> fnCallback);

	std::map<InteractionEventType, std::function<RESULT(void*)>> m_callbacks;

	RESULT Notify(InteractionObjectEvent *event) override;

// Menu Controller Observer
	RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode);

private:
	//Cloud member variables
	CloudController *m_pCloudController = nullptr;
	MenuControllerProxy *m_pMenuControllerProxy = nullptr;
	std::shared_ptr<MenuNode> m_pMenuNode = nullptr;
};


#endif // ! DREAM_UI_BAR_H_