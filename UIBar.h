#ifndef UI_BAR_H_
#define UI_BAR_H_

#include "Primitives/composite.h"
#include "Primitives/ray.h"

#include "Sense/SenseController.h"
#include "Primitives/Publisher.h"

#include <stack>

typedef struct UIBarInfo {
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
} UI_BAR_INFO;

class UIBar : public Subscriber<SenseControllerEvent> {
public:
	UIBar(composite* c);
	UIBar(composite* c, UIBarInfo info);
	~UIBar();

	RESULT Initialize();

	virtual RESULT Notify(SenseControllerEvent *event) override;
	RESULT Update(ray handRay);

	RESULT ToggleVisible();

private:
	composite *m_context;

	// Update visible menu based on menu title
	// Access the menu map, where currently title is the key and menu items are the values
	RESULT DisplayFromMenuTitle(std::string title);

	RESULT UpdateSelectedItem(int index);

	// returns location of furthest point in ray/sphere collision
	// this code should be in boundingsphere, but it is written here to avoid conflicts for now
	point FurthestRaySphereIntersect(const ray &r, point center);

private:
	bool m_UIDirty;
	bool m_UISelect;

	float m_rotationY;
	int m_selectedIndex;

	std::vector<std::shared_ptr<composite>> m_buttons;
	int m_visibleMenuItems;

	UIBarInfo m_info;
	std::stack<std::string> m_menuPath;
};


#endif // ! UI_BAR_H