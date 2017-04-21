#ifndef UI_MODULE_H_
#define UI_MODULE_H_

#include "Primitives/valid.h"
#include "Primitives/composite.h"
#include "UIMenuLayer.h"
#include "Primitives/Subscriber.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include <stack>

class DreamOS;
class CloudController;

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
	// using virtual here causes crash on exit
	UIModule();
	~UIModule();

	RESULT Initialize(composite *pComposite);
	ray GetHandRay(hand* pHand);
	std::shared_ptr<UIMenuLayer> CreateMenuLayer();
	std::shared_ptr<UIMenuLayer> GetCurrentLayer();

	std::shared_ptr<UIMenuItem> GetMenuItem(VirtualObj* pObj);
	quaternion GetInitialMenuOrientation();

	RESULT ToggleVisible();
	RESULT Show();
	RESULT Hide();
	RESULT SetVisible(bool fVisible);
	bool IsVisible();

protected:
	composite* m_pCompositeContext;

	std::vector<std::shared_ptr<UIMenuLayer>> m_layers;

	//TODO: when multiple layers are needed, implement currentUILayer 
	// as an iterator with public Set/Previous/Next functions
	std::shared_ptr<UIMenuLayer> m_pCurrentUILayer;

	float m_headRotationYDeg;  // can be removed with composite collision code
};

#endif // ! UI_MODULE_H_