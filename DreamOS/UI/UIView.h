#ifndef UI_VIEW_H_
#define UI_VIEW_H_

#include "Primitives/Publisher.h"
#include "Primitives/Subscriber.h"

#include "InteractionEngine/InteractionEngine.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "UIEvent.h"

#include "Primitives/composite.h"

class UIButton;
class UISpatialScrollView;
class UIFlatScrollView;
class UIMenuItem;
class UserAreaControls;
class UISurface;
class UITabView;
class UIControlView;
class DreamOS;

class UIView : public composite, public Publisher<UIEventType, UIEvent>, public Subscriber<InteractionObjectEvent> {
public:
	UIView(HALImp *pHALImp, DreamOS *pDreamOS);
	UIView(HALImp *pHALImp, DreamOS *pDreamOS, float width, float height);
	~UIView();

	RESULT Initialize();

public:

	std::shared_ptr<UIView> MakeUIView();
	std::shared_ptr<UIView> AddUIView();

	std::shared_ptr<UIButton> MakeUIButton();
	std::shared_ptr<UIButton> AddUIButton();

	std::shared_ptr<UIButton> MakeUIButton(float width, float height);
	std::shared_ptr<UIButton> AddUIButton(float width, float height);

	std::shared_ptr<UIButton> MakeUIButton(std::shared_ptr<texture> pEnabledTexture, std::shared_ptr<texture>, float width, float height);
	std::shared_ptr<UIButton> AddUIButton(std::shared_ptr<texture> pEnabledTexture, std::shared_ptr<texture>, float width, float height);

	std::shared_ptr<UIButton> AddButton(float offset, float width, float height, std::function<RESULT(UIButton*, void*)> fnCallback, std::shared_ptr<texture> pEnabledTexture = nullptr, std::shared_ptr<texture> pDisabledTexture = nullptr);

	std::shared_ptr<UIMenuItem> MakeUIMenuItem();
	std::shared_ptr<UIMenuItem> AddUIMenuItem();

	std::shared_ptr<UIMenuItem> MakeUIMenuItem(float width, float height);
	std::shared_ptr<UIMenuItem> AddUIUIMenuItem(float width, float height);

	std::shared_ptr<UserAreaControls> MakeUIContentControlBar();
	std::shared_ptr<UserAreaControls> AddUIContentControlBar();

	std::shared_ptr<UITabView> MakeUITabView();
	std::shared_ptr<UITabView> AddUITabView();

	std::shared_ptr<UISpatialScrollView> MakeUISpatialScrollView();
	std::shared_ptr<UISpatialScrollView> AddUISpatialScrollView();

	std::shared_ptr<UIFlatScrollView> MakeUIFlatScrollView();
	std::shared_ptr<UIFlatScrollView> AddUIFlatScrollView();
	
	std::shared_ptr<UIControlView> MakeUIControlView();
	std::shared_ptr<UIControlView> AddUIControlView();

	std::shared_ptr<UISurface> MakeUISurface();
	std::shared_ptr<UISurface> AddUISurface();

	virtual RESULT Notify(InteractionObjectEvent *pEvent);

protected:
	DreamOS *m_pDreamOS = nullptr;

	float m_width;
	float m_height;
};

#endif // !UI_VIEW_H_ 