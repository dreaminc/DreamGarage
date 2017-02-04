#ifndef UI_MODULE_H_
#define UI_MODULE_H_

#include "Primitives/composite.h"
#include "UIMenuLayer.h"
#include "Sense/SenseController.h"
#include "Primitives/Publisher.h"

class UIModule : public Subscriber<SenseControllerEvent> {
public:
	UIModule(composite* c);
	~UIModule();

	RESULT Initialize();

	virtual RESULT Notify(SenseControllerEvent *event) override = 0;

	RESULT CreateMenuLayer(int numButtons);

protected:
	composite* m_context;

	std::vector<UIMenuLayer*> m_pLayers;
};

#endif // ! UI_MODULE_H_