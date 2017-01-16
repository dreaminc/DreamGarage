#ifndef UI_BAR_H_
#define UI_BAR_H_

#include "Primitives/composite.h"
#include "Primitives/ray.h"

#include "Sense/SenseController.h"

class UIBar : public Subscriber<SenseControllerEvent> {
public:
	UIBar(composite* c);
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
	vector m_UIScale;
	float m_enlargedScale;
	int m_selectedIndex;
	int m_numButtons;
	
	float m_depth;

	float m_angleY;

};


#endif // ! UI_BAR_H