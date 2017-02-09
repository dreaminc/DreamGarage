#ifndef UI_BAR_H_
#define UI_BAR_H_

#include "Primitives/composite.h"
#include "Primitives/ray.h"

#include "Sense/SenseController.h"
#include "Primitives/Publisher.h"

#include "UIModule.h"

#include "Primitives/dirty.h"

#include <stack>

typedef struct UIBarFormat {
	float yPosition;
	float menuDepth;

	float itemAngleX;
	float itemAngleY;
	vector itemScale;
	float enlargedScale;

	float headerAngleX;
	float headerYPos;

	UIBarFormat() :
		yPosition(-0.5f),
		menuDepth(-1.5f),
		itemAngleX(60.0f),
		itemAngleY(20.0f),
		itemScale(vector(1.0f, 1.0f, 1.0f)),
		enlargedScale(1.25f),
		headerAngleX(75.0f),
		headerYPos(0.0f)
	{}

} UI_BAR_INFO;

class UIBar : public UIModule {
public:
	UIBar(composite* pComposite, UIMenuItem::IconFormat iconFormat, UIMenuItem::LabelFormat labelFormat);
	~UIBar();

	virtual RESULT HandleMenuUp(UILayerInfo info) override;
	virtual RESULT HandleTriggerUp(UILayerInfo info) override;

	RESULT Update(ray handRay);

	virtual RESULT UpdateCurrentUILayer(UILayerInfo info) override;

	RESULT ToggleVisible();
	int GetSelectedIndex();

private:

	// Places MenuItem along a circular arc based on index
	RESULT UpdateWithRadialLayout(std::shared_ptr<UIMenuItem> pItem, int index, int size, bool fHeader);

	// Updates MenuItem scale based on a new selected index
	RESULT UpdateSelectedItem(int index, int size);

	// returns location of furthest point in ray/sphere collision
	// this code should be in boundingsphere, but it is written here to avoid conflicts for now
	point FurthestRaySphereIntersect(const ray &r, point center, float radius);


private:
	// these flags help detect controller 'up' events
	bool m_UISelect;

	float m_headRotationYDeg;
	int m_selectedIndex;

	int m_visibleMenuItems;

	UIMenuItem::IconFormat m_iconFormat;
	UIMenuItem::LabelFormat m_labelFormat;

	std::stack<std::string> m_menuPath;
};


#endif // ! UI_BAR_H