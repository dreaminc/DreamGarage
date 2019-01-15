#ifndef UI_BUTTON_H_
#define UI_BUTTON_H_

#include "UIView.h"
#include <functional>

class DreamOS;

#define DEFAULT_WIDTH 0.25f
#define ASPECT_RATIO (9.0f / 16.0f)
#define DEFAULT_HEIGHT (DEFAULT_WIDTH * ASPECT_RATIO)

class UIButton : public UIView, public Subscriber<UIEvent> {
public:
	UIButton(HALImp *pHALImp,
		DreamOS *pDreamOS,
		float width = DEFAULT_WIDTH,
		float height = DEFAULT_HEIGHT,
		std::shared_ptr<texture> pEnabledTexture = nullptr,
		std::shared_ptr<texture> pDisabledTexture = nullptr);

	~UIButton();

	RESULT Initialize();

public:
	RESULT RegisterToInteractionEngine(DreamOS *pDreamOS);
	RESULT RegisterEvent(UIEventType type, std::function<RESULT(UIButton*,void*)> fnCallback, void *pContext = nullptr);
	RESULT Notify(UIEvent *pEvent);

	std::shared_ptr<composite> GetSurfaceComposite();
	std::shared_ptr<quad> GetSurface();
	VirtualObj *GetInteractionObject();
	point GetContactPoint();

public:
	RESULT HandleTouchStart(UIButton* pButtonContext, void* pContext);
	// temp hopefully
	RESULT RegisterTouchStart();

	RESULT SetEnabledFlag(bool fEnabled);
	RESULT Toggle();
	bool IsToggled();

	RESULT SwitchToTexture(bool fIsEnabledTexture);

	RESULT SetEnabledTexture(std::shared_ptr<texture> pEnabledTexture);
	RESULT SetDisabledTexture(std::shared_ptr<texture> pDisabledTexture);

protected:
	// objects (TODO: could be in subclass)

	// quad registered in interaction events
	std::shared_ptr<quad> m_pSurface = nullptr;

	// context for positioning button objects
	std::shared_ptr<composite> m_pSurfaceComposite = nullptr;

	std::map<UIEventType, std::pair<std::function<RESULT(UIButton*,void*)>,void*>> m_callbacks;

	VirtualObj *m_pInteractionObject = nullptr; // set on UIEvents since the button is the context for the callbacks
	point m_ptContact;

	// whether the button is enabled influences the interaction events
	bool m_fEnabled = true;

	std::shared_ptr<texture> m_pEnabledTexture = nullptr;
	std::shared_ptr<texture> m_pDisabledTexture = nullptr;

};

#endif // ! UI_BUTTON_H_
