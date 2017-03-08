#ifndef UI_MENU_LAYER_H_
#define UI_MENU_LAYER_H_

#include "Primitives/valid.h"
#include "Primitives/composite.h"
#include "UIMenuItem.h"

class UIMenuLayer : public valid{
public:
	UIMenuLayer(composite* pParentContext);
	~UIMenuLayer();

	RESULT Initialize();

	std::shared_ptr<UIMenuItem> CreateMenuItem();
	RESULT Clear();

	std::vector<std::shared_ptr<UIMenuItem>> GetMenuItems();

	//Returns the first instance of a MenuItem that contains the argument 
	std::shared_ptr<UIMenuItem> GetMenuItem(VirtualObj *pObj);

private:
	std::shared_ptr<composite> m_pContextComposite; // composite for creation of Menu Items
	std::vector<std::shared_ptr<UIMenuItem>> m_menuItems;

private:
	composite *m_pParentContext = nullptr;
};


#endif // ! UI_MENU_LAYER_H_