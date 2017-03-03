#ifndef UI_MODULE_H_
#define UI_MODULE_H_

#include "Primitives/valid.h"
#include "Primitives/composite.h"
#include "UIMenuLayer.h"

#include <stack>

class DreamOS;

typedef struct UILayerInfo {
	std::vector<std::shared_ptr<texture>> icons;
	std::vector<std::string> labels;
	UILayerInfo() :
		icons({}),
		labels({})
	{}
} UI_LAYER_INFO;

class UIModule : public valid {
public:
	// using virtual here causes crash on exit
	UIModule(DreamOS *pDreamOS);
	~UIModule();

	RESULT Initialize();

	virtual RESULT HandleMenuUp(std::map<std::string, std::vector<std::string>>& menu, std::stack<std::string>& path) = 0;
	virtual RESULT HandleTriggerUp(VirtualObj* prev, std::map<std::string, std::vector<std::string>>& menu, std::stack<std::string>& path) = 0;

	std::shared_ptr<UIMenuLayer> CreateMenuLayer();
	std::shared_ptr<UIMenuLayer> GetCurrentLayer();

	virtual RESULT UpdateCurrentUILayer(UILayerInfo& info) = 0;

	RESULT ToggleVisible();
	RESULT Show();
	RESULT Hide();
	RESULT SetVisible(bool fVisible);
	bool IsVisible();

	composite* GetComposite();

protected:
	composite* m_pCompositeContext;

	std::vector<std::shared_ptr<UIMenuLayer>> m_layers;

	//TODO: when multiple layers are needed, implement currentUILayer 
	// as an iterator with public Set/Previous/Next functions
	std::shared_ptr<UIMenuLayer> m_pCurrentUILayer;

	float m_headRotationYDeg;  // can be removed with composite collision code

private:
	DreamOS *m_pDreamOS;
};

#endif // ! UI_MODULE_H_