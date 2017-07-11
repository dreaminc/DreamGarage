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

	std::shared_ptr<composite> GetContextComposite();
	std::shared_ptr<composite> GetSurfaceComposite();
	std::shared_ptr<quad> GetSurface();
	VirtualObj *GetInteractionObject();

protected:
	// objects (TODO: could be in subclass)

	// quad registered in interaction events
	std::shared_ptr<quad> m_pSurface = nullptr;

	// context for positioning button objects
	std::shared_ptr<composite> m_pSurfaceComposite = nullptr;

	// context for positioning button
	std::shared_ptr<composite> m_pContextComposite = nullptr;

	std::map<UIEventType, std::function<RESULT(void*)>> m_callbacks;

	VirtualObj *m_pInteractionObject = nullptr; // set on UIEvents since the button is the context for the callbacks
};

#endif // ! UI_BUTTON_H_
