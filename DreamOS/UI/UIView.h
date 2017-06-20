#ifndef UI_VIEW_H_
#define UI_VIEW_H_

#include "Primitives/Publisher.h"
#include "Primitives/Subscriber.h"

#include "InteractionEngine/InteractionEngine.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "UIEvent.h"

#include "Primitives/composite.h"

class UIButton;
class UIScrollView;
class UIMenuItem;

class UIView : public composite, public Publisher<UIEventType, UIEvent>, public Subscriber<InteractionObjectEvent> {
public:
	UIView(HALImp *pHALImp);
	~UIView();

	RESULT Initialize();

public:

	//TODO: these could be MakeUIObject<T>
	std::shared_ptr<UIButton> MakeUIButton();
	std::shared_ptr<UIButton> AddUIButton();

	std::shared_ptr<UIMenuItem> MakeUIMenuItem();
	std::shared_ptr<UIMenuItem> AddUIMenuItem();

	std::shared_ptr<UIScrollView> MakeUIScrollView();
	std::shared_ptr<UIScrollView> AddUIScrollView();

	RESULT Notify(InteractionObjectEvent *pEvent);
};

#endif // !UI_VIEW_H_ 