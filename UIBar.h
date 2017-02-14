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

	float menuPosZ;

	// item
	float itemAngleX;
	float itemAngleY; // angle between items
	float itemPosY;
	vector itemScale;
	float itemScaleSelected;

	// header 
	float headerAngleX;
	float headerPosY;

	UIBarFormat() :
		menuPosZ(-1.5f),
		itemAngleX(60.0f),
		itemAngleY(20.0f),
		itemPosY(-0.5f),
		itemScale(vector(1.0f, 1.0f, 1.0f)),
		itemScaleSelected(1.25f),
		headerAngleX(75.0f),
		headerPosY(0.0f)
	{}

} UI_BAR_INFO;

class UIBar : public UIModule {
public:
	UIBar(composite* pComposite, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat);
	~UIBar();

	virtual RESULT HandleMenuUp(UILayerInfo& info) override;
	virtual RESULT HandleTriggerUp(UILayerInfo& info) override;


	virtual RESULT UpdateCurrentUILayer(UILayerInfo& info) override;

	// TODO: these functions can be removed/replaced once there is composite collision code
	RESULT Update(ray handRay);
	size_t GetSelectedIndex();

private:

	int GetIndexFromRay(ray handRay);

	// Places MenuItem along a circular arc based on index
	RESULT UpdateWithRadialLayout(size_t index);

	// TODO: these functions can be removed/replaced once there is composite collision code
	// Updates MenuItem scale based on a new selected index
	RESULT UpdateSelectedItem(size_t index);

	// returns location of furthest point in ray/sphere collision
	point FurthestRaySphereIntersect(const ray &r, point center, float radius);


private:
	// these flags help detect controller 'up' events
	bool m_UISelect;

	size_t m_selectedIndex;

	IconFormat m_iconFormat;
	LabelFormat m_labelFormat;
	UIBarFormat m_barFormat;

	std::stack<std::string> m_menuPath;
};


#endif // ! UI_BAR_H