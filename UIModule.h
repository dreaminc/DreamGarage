#ifndef UI_MODULE_H_
#define UI_MODULE_H_

#include "Primitives/composite.h"
#include "UIMenuLayer.h"
//#include "Sense/SenseController.h"
//#include "Primitives/Publisher.h"

typedef struct UILayerInfo {
	std::vector<std::shared_ptr<texture>> icons;
	std::vector<std::string> labels;
	UILayerInfo() :
		icons({}),
		labels({})
	{}
} UI_LAYER_INFO;

class UIModule {// : public Subscriber<SenseControllerEvent> {
public:
	UIModule(composite* pComposite);
	~UIModule();

	virtual RESULT HandleMenuUp(UILayerInfo info) = 0;
	virtual RESULT HandleTriggerUp(UILayerInfo info) = 0;

	std::shared_ptr<UIMenuLayer> CreateMenuLayer();

	virtual RESULT UpdateCurrentUILayer(UILayerInfo info) = 0;

//	RESULT SetCurrentLayer(std::shared_ptr<UIMenuLayer> pLayer);
//	RESULT NextLayer();
//	RESULT PreviousLayer();

protected:
	composite* m_pContext;

	std::vector<std::shared_ptr<UIMenuLayer>> m_layers;
//	std::vector<std::shared_ptr<UIMenuLayer>>::iterator m_currentUILayer;
	std::shared_ptr<UIMenuLayer> m_currentUILayer;
};

#endif // ! UI_MODULE_H_