#ifndef UI_MENU_ITEM_SUBSCRIBER_H_
#define UI_MENU_ITEM_SUBSCRIBER_H_

#include "UIModule.h"
#include <functional>

class UIMenuItemSubscriber
{
public:
	UIMenuItemSubscriber();
	~UIMenuItemSubscriber();
private:
//	UIMenuItemEvent type;
	void* m_pContext;
	//std::function<RESULT(UIModule::UIMenuItemEvent type, void* pContext)> m_fnCallback;
	std::function<RESULT()> m_fnCallback;
};

#endif // ! UI_MENU_ITEM_SUBSCRIBER_H_