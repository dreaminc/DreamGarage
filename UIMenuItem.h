#ifndef UI_MENU_ITEM_H_
#define UI_MENU_ITEM_H_

#include "Primitives/FlatContext.h"
#include "Primitives/composite.h"

class UIMenuItem {
public:
	UIMenuItem(composite* c);
	~UIMenuItem();

	RESULT Initialize();

	std::shared_ptr<composite> GetButton();
	std::shared_ptr<quad> GetQuad();

private:
	composite *m_context;

	std::shared_ptr<composite> m_pButton;
	std::shared_ptr<quad> m_pQuad;

};


#endif // ! UI_MENU_ITEM_H_