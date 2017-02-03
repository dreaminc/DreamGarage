#ifndef UI_MENU_LAYER_H_
#define UI_MENU_LAYER_H_

#include "Primitives/composite.h"
#include "UIMenuItem.h"

class UIMenuLayer {
public:
	UIMenuLayer(composite* c);
	~UIMenuLayer();

	RESULT Initialize();
	std::shared_ptr<UIMenuItem> CreateMenuItem();
	RESULT Clear();

private:
	composite *m_context; // composite for creation of Menu Items
};


#endif // ! UI_MENU_LAYER_H_