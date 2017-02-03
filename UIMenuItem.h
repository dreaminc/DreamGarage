#ifndef UI_MENU_ITEM_H_
#define UI_MENU_ITEM_H_

#include "Primitives/FlatContext.h"
#include "Primitives/composite.h"

class UIMenuItem {
public:
	UIMenuItem(composite* c);
	~UIMenuItem();

	RESULT Initialize();

private:
	composite *m_context;
	FlatContext *m_pFlatContext;
};


#endif // ! UI_MENU_ITEM_H_