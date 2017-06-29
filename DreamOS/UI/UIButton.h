#ifndef UI_BUTTON_H_
#define UI_BUTTON_H_

#include "UIView.h"
#include <functional>

class DreamOS;

class UIButton : public UIView, public Subscriber<UIEvent> {
public:
	UIButton(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIButton();

	RESULT Initialize();

public:
	RESULT RegisterToInteractionEngine(DreamOS *pDreamOS);
	RESULT RegisterEvent(UIEventType type, std::function<RESULT(void*)> fnCallback);
	RESULT Notify(UIEvent *pEvent);

	std::shared_ptr<composite> GetComposite();
	std::shared_ptr<quad> GetSurface();

protected:
	// objects (TODO: could be in subclass)
	std::shared_ptr<quad> m_pSurface = nullptr;
	std::shared_ptr<composite> m_pContextComposite = nullptr;
	std::map<UIEventType, std::function<RESULT(void*)>> m_callbacks;
};

#endif // ! UI_BUTTON_H_
