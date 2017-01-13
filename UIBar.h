#ifndef UI_BAR_H_
#define UI_BAR_H_

#include "Primitives/composite.h"
#include "Sense/SenseController.h"

class UIBar : public Subscriber<SenseControllerEvent> {
public:
	UIBar(composite* c);
	~UIBar();

	RESULT Initialize();

	virtual RESULT Notify(SenseControllerEvent *event) override;
	RESULT Update(float handRotation = NULL);

	RESULT ToggleVisible();

private:
	composite *m_context;

private:
	bool m_UIDirty;
	float m_rotationY;
	vector m_UIScale;
	int m_selectedIndex;
	int m_numButtons;

};


#endif // ! UI_BAR_H