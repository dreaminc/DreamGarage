#ifndef UI_BAR_H_
#define UI_BAR_H_

#include "Primitives/composite.h"
#include "Primitives/ray.h"

#include "Sense/SenseController.h"

typedef struct UIBarInfo {
	int maxNumButtons;
	float yPosition;
	float menuDepth;
	float itemAngleX;
	float itemAngleY;
	vector itemScale;
	float enlargedScale;
	std::vector<std::string> words;
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

	// returns location of furthest point in ray/sphere collision
	// this code should be in boundingsphere, but it is written here to avoid conflicts for now
	point FurthestRaySphereIntersect(const ray &r, point center);

private:
	bool m_UIDirty;
	float m_rotationY;
	int m_selectedIndex;

	UIBarInfo m_info;

};


#endif // ! UI_BAR_H