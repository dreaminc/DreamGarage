#ifndef UI_MALLET_H_
#define UI_MALLET_H_
#include "RESULT/EHM.h"
#include "Primitives/point.h"
#include "Primitives/dirty.h"

#define MALLET_RADIUS 0.015f

class DreamOS;
class sphere;
class point;

class UIMallet : public dirty {
public:
	UIMallet(DreamOS *pDreamOS);
	~UIMallet();

public:
	RESULT Show();
	RESULT Hide();
	sphere* GetMalletHead();
	float GetRadius();
	point GetHeadOffset();

private:
	float m_radius;
	float m_distance;
	float m_angle;
	sphere *m_pHead;
	point m_headOffset;
	DreamOS *m_pDreamOS;
};

#endif // ! UI_MALLET_H_
