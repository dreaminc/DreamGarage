#ifndef UI_MODULE_H_
#define UI_MODULE_H_

#include "Primitives/composite.h"
#include "UIMenuLayer.h"
#include "Sense/SenseController.h"
#include "Primitives/Publisher.h"

class UIModule : public Subscriber<SenseControllerEvent> {
public:
	UIModule(composite* pComposite);
	~UIModule();

	virtual RESULT Notify(SenseControllerEvent *event) = 0;

	std::shared_ptr<UIMenuLayer> CreateMenuLayer();

	virtual RESULT UpdateCurrentUILayer() = 0;

	RESULT SetCurrentLayer(std::shared_ptr<UIMenuLayer> pLayer);
	RESULT NextLayer();
	RESULT PreviousLayer();

protected:
	composite* m_pContext;

	std::vector<std::shared_ptr<UIMenuLayer>> m_layers;
	std::vector<std::shared_ptr<UIMenuLayer>>::iterator m_currentUILayer;
};

#endif // ! UI_MODULE_H_