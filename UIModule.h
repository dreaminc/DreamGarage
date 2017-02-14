#ifndef UI_MODULE_H_
#define UI_MODULE_H_

#include "Primitives/composite.h"
#include "UIMenuLayer.h"

typedef struct UILayerInfo {
	std::vector<std::shared_ptr<texture>> icons;
	std::vector<std::string> labels;
	UILayerInfo() :
		icons({}),
		labels({})
	{}
} UI_LAYER_INFO;

class UIModule {
public:
	UIModule(composite* pComposite);
	// using virtual here causes crash on exit
	~UIModule();

	virtual RESULT HandleMenuUp(UILayerInfo& info) = 0;
	virtual RESULT HandleTriggerUp(UILayerInfo& info) = 0;

	std::shared_ptr<UIMenuLayer> CreateMenuLayer();

	virtual RESULT UpdateCurrentUILayer(UILayerInfo& info) = 0;

	RESULT ToggleVisible();

protected:
	composite* m_pContext;

	std::vector<std::shared_ptr<UIMenuLayer>> m_layers;

	//TODO: when multiple layers are needed, implement currentUILayer 
	// as an iterator with public Set/Previous/Next functions
	std::shared_ptr<UIMenuLayer> m_currentUILayer;

	float m_headRotationYDeg;  // can be removed with composite collision code
};

#endif // ! UI_MODULE_H_