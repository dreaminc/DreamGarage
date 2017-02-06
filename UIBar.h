#ifndef UI_BAR_H_
#define UI_BAR_H_

#include "Primitives/composite.h"
#include "Primitives/ray.h"

#include "Sense/SenseController.h"
#include "Primitives/Publisher.h"

#include "UIModule.h"

#include <stack>

typedef struct UIBarFormat {
	int maxNumButtons;		
	float yPosition;
	float menuDepth;

	float itemAngleX;
	float itemAngleY;
	vector itemScale;
	float enlargedScale;

	float headerAngleX;
	float headerYPos;

	std::map<std::string, std::vector<std::string>> menu;

	UIBarFormat() :
		maxNumButtons(5),
		yPosition(-0.5f),
		menuDepth(-1.5f),
		itemAngleX(60.0f),
		itemAngleY(20.0f),
		itemScale(vector(1.0f, 1.0f, 1.0f)),
		enlargedScale(1.25f),
		headerAngleX(75.0f),
		headerYPos(0.0f),
		menu({})
	{}

} UI_BAR_INFO;

class UIBar : public UIModule {
public:
	UIBar(composite* c, UIBarFormat info = UIBarFormat());
	~UIBar();

	RESULT Initialize();

	virtual RESULT Notify(SenseControllerEvent *event) override;
	RESULT Update(ray handRay);

	RESULT ToggleVisible();

private:
//	composite *m_context;

	// Update visible menu based on menu title
	// Access the menu map, where currently title is the key and menu items are the values
	RESULT DisplayFromMenuTitle(std::string title);

	// Places MenuItem along a circular arc based on index
	RESULT UpdateWithRadialLayout(std::shared_ptr<UIMenuItem> pItem, int index);

	// Updates MenuItem scale based on a new selected index
	RESULT UpdateSelectedItem(int index);

	// returns location of furthest point in ray/sphere collision
	// this code should be in boundingsphere, but it is written here to avoid conflicts for now
	point FurthestRaySphereIntersect(const ray &r, point center);

private:
	// these flags help detect controller 'up' events
	bool m_UIDirty;
	bool m_UISelect;

	float m_rotationY;
	int m_selectedIndex;

	int m_visibleMenuItems;

	UIBarFormat m_info;
	std::stack<std::string> m_menuPath;
};


#endif // ! UI_BAR_H