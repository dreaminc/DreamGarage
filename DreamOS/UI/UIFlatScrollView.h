#ifndef UI_FLAT_SCROLL_VIEW_H_
#define UI_FLAT_SCROLL_VIEW_H_

#include "UIView.h"
#include "UIScrollView.h"
#include "Primitives/Subscriber.h"
#include "Sense/SenseController.h"
#include "Primitives/FlatContext.h"

class UIButton;
class DreamOS;
//class FlatContext;

class UIFlatScrollView : public UIView, public UIScrollView//, public Subscriber<SenseControllerEvent>
{
	friend class MultiContentTestSuite;
public:
	UIFlatScrollView(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIFlatScrollView();

	RESULT Update();

// manage button list
public:
	std::vector<std::shared_ptr<UIButton>> GetTabButtons();
	texture *GetCurrentTexture();

	RESULT SetScrollFlag(bool fCanScroll, int index);

	RESULT SetBounds(float width, float height);

	std::shared_ptr<FlatContext> GetRenderContext();
	RESULT SetRenderQuad(std::shared_ptr<quad> pRenderQuad);

public:
	RESULT Notify(SenseControllerEvent *pEvent);

private:

	std::vector<std::shared_ptr<UIButton>> m_pTabButtons;

	std::shared_ptr<FlatContext> m_pRenderContext = nullptr;

	std::shared_ptr<quad> m_pRenderQuad = nullptr;

	texture *m_pCurrentTexture = nullptr;

	bool m_fCanScrollFlag[2];
};

#endif // ! UI_FLAT_SCROLL_VIEW_H_