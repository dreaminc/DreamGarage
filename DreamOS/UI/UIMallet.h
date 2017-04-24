#ifndef UI_MALLET_H_
#define UI_MALLET_H_
#include "RESULT/EHM.h"
#include "Primitives/point.h"

class DreamOS;
class sphere;
class point;

class UIMallet {
public:
	UIMallet(DreamOS *pDreamOS);
	~UIMallet();

public:
	RESULT Show();
	RESULT Hide();
	sphere* GetMalletHead();
	point GetHeadOffset();

private:
	sphere *m_pHead;
	point m_headOffset;
	DreamOS *m_pDreamOS;
};

#endif // ! UI_MALLET_H_
