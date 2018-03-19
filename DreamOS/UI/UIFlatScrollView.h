#ifndef UI_FLAT_SCROLL_VIEW_H_
#define UI_FLAT_SCROLL_VIEW_H_

#include "UIView.h"
#include "UIScrollView.h"
#include "Primitives/Subscriber.h"
#include "Sense/SenseController.h"

class UIButton;
class DreamOS;
class FlatContext;

class UIFlatScrollView : public UIView, public UIScrollView//, public Subscriber<SenseControllerEvent>
{
public:
	UIFlatScrollView(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIFlatScrollView();

	RESULT Update();

// manage button list
public:
	std::vector<std::shared_ptr<UIButton>> GetTabButtons();

	RESULT SetScrollFlag(bool fCanScroll, int index);

public:
	RESULT Notify(SenseControllerEvent *pEvent);

private:

	std::vector<std::shared_ptr<UIButton>> m_pTabButtons;

	FlatContext *m_pRenderContext = nullptr;

	bool m_fCanScrollFlag[2];
};

#endif // ! UI_FLAT_SCROLL_VIEW_H_