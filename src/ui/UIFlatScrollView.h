#ifndef UI_FLAT_SCROLL_VIEW_H_
#define UI_FLAT_SCROLL_VIEW_H_

#include "UIView.h"
#include "UIScrollView.h"
#include "Primitives/Subscriber.h"
#include "Sense/SenseController.h"
#include "Primitives/FlatContext.h"

class UIButton;
class DreamOS;
class DreamContentSource;
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
	bool CanScroll();

	RESULT SetBounds(float width, float height);

	std::shared_ptr<FlatContext> GetRenderContext();
	RESULT SetRenderQuad(std::shared_ptr<quad> pRenderQuad, std::shared_ptr<FlatContext> pFlatContext);

	RESULT SetTabWidth(float tabWidth);
	RESULT SetTabHeight(float tabHeight);

	RESULT SetScrollSnapDistance(float snapDistance);
	RESULT Snap();

public:
	std::shared_ptr<UIButton> CreateTab(DreamOS *pDreamOS, std::shared_ptr<DreamContentSource> pContent, point ptPosition);

public:
	RESULT Notify(SenseControllerEvent *pEvent);

private:

	std::vector<std::shared_ptr<UIButton>> m_pTabButtons;

	std::shared_ptr<FlatContext> m_pRenderContext = nullptr;
	std::shared_ptr<quad> m_pRenderQuad = nullptr;

	texture *m_pCurrentTexture = nullptr;

	bool m_fCanScrollFlag[2] = { true, true };

	int m_currentIndex = 0;
	float m_scrollSnapDistance = 1.0f;

	float m_tabWidth = 0.0f;
	float m_tabHeight = 0.0f;

};

#endif // ! UI_FLAT_SCROLL_VIEW_H_