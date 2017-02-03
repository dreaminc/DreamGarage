#ifndef UI_MODULE_H_
#define UI_MODULE_H_

#include "Primitives/composite.h"
#include "UIMenuLayer.h"

class UIModule {
public:
	UIModule(composite* c);
	~UIModule();

	RESULT Initialize();

private:
	composite* m_context;

	std::vector<UIMenuLayer> m_pLayers;
};

#endif // ! UI_MODULE_H_