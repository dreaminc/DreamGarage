#ifndef UI_VIEW_H_
#define UI_VIEW_H_

#include "Primitives/Publisher.h"
#include "Primitives/Subscriber.h"

#include "InteractionEngine/InteractionEngine.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "UIEvent.h"

#include "Primitives/composite.h"

class UIButton;

class UIView : public composite, public Publisher<UIEventType, UIEvent>, public Subscriber<InteractionObjectEvent> {
public:
	UIView(HALImp *pHALImp);
	~UIView();

public:

	std::shared_ptr<UIButton> MakeUIButton();
	std::shared_ptr<UIButton> AddUIButton();

	RESULT Notify(InteractionObjectEvent *pEvent);

};

#endif // !UI_VIEW_H_ 