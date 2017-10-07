#ifndef UI_MALLET_H_
#define UI_MALLET_H_
#include "RESULT/EHM.h"
#include "Primitives/point.h"
#include "Primitives/dirty.h"

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
	RESULT SetHeadOffset(point p);
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
