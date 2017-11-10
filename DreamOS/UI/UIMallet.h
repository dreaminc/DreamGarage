#ifndef UI_MALLET_H_
#define UI_MALLET_H_
#include "RESULT/EHM.h"
#include "Primitives/point.h"
#include "Primitives/dirty.h"

#define MALLET_ANGLE 203.0f * (float)(M_PI) / 180.0f
#define MALLET_RADIUS 0.2f

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
	sphere *m_pHead;
	point m_headOffset;
	DreamOS *m_pDreamOS;
};

#endif // ! UI_MALLET_H_
