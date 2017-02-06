#ifndef UI_MENU_LAYER_H_
#define UI_MENU_LAYER_H_

#include "Primitives/composite.h"
#include "UIMenuItem.h"

class UIMenuLayer {
public:
	UIMenuLayer(composite* c);
	~UIMenuLayer();

	std::shared_ptr<UIMenuItem> CreateMenuItem();
	RESULT Clear();

	std::vector<std::shared_ptr<UIMenuItem>> GetMenuItems();

private:
	composite *m_context; // composite for creation of Menu Items
	std::vector<std::shared_ptr<UIMenuItem>> m_pMenuItems;
};


#endif // ! UI_MENU_LAYER_H_