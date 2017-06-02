#ifndef UI_VIEW_H_
#define UI_VIEW_H_

#include "Primitives/Publisher.h"
#include "Primitives/Subscriber.h"

#include "InteractionEngine/InteractionEngine.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "UIEvent.h"

#include "Primitives/composite.h"

class UIView : public composite, public Publisher<UIEventType, UIEvent>, public Subscriber<InteractionObjectEvent> {
public:
	UIView(HALImp *pHALImp);
	~UIView();

public:
	RESULT Notify(InteractionObjectEvent *pEvent);

};

#endif // !UI_VIEW_H_ 