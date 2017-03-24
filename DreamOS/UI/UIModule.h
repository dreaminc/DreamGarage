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

class UIModule : public valid, public Subscriber<InteractionObjectEvent> {
public:
	// using virtual here causes crash on exit
	UIModule(DreamOS *pDreamOS);
	~UIModule();

	RESULT Initialize();
	ray GetHandRay();
	RESULT UpdateInteractionPrimitive(ray rCast);

	std::shared_ptr<UIMenuLayer> CreateMenuLayer();
	std::shared_ptr<UIMenuLayer> GetCurrentLayer();

	virtual RESULT UpdateCurrentUILayer(UILayerInfo& info) = 0;

	std::shared_ptr<UIMenuItem> GetMenuItem(VirtualObj* pObj);
	std::shared_ptr<UIMenuItem> GetCurrentItem();

	RESULT ToggleVisible();
	RESULT Show();
	RESULT Hide();
	RESULT SetVisible(bool fVisible);
	bool IsVisible();

	composite* GetComposite();

	virtual RESULT Notify(InteractionObjectEvent *event) override;

protected:
	composite* m_pCompositeContext;

	std::vector<std::shared_ptr<UIMenuLayer>> m_layers;

	//TODO: when multiple layers are needed, implement currentUILayer 
	// as an iterator with public Set/Previous/Next functions
	std::shared_ptr<UIMenuLayer> m_pCurrentUILayer;

	std::shared_ptr<UIMenuItem> m_pCurrentItem;

	float m_headRotationYDeg;  // can be removed with composite collision code

	CloudController *GetCloudController();

private:
	DreamOS *m_pDreamOS;

	DimRay *m_pTestRayController;
	DimRay *m_pTestRayLookV;
};

#endif // ! UI_MODULE_H_