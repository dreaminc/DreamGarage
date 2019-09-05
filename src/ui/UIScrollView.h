#ifndef UI_SCROLL_VIEW_H_
#define UI_SCROLL_VIEW_H_

#include "core/ehm/EHM.h"

// Dream UI
// dos/src/ui/UIScrollView.h

#include "UIView.h"

#include "core/types/Subscriber.h"

#define PAD_MOVE_CONSTANT 0.015f

class UIScrollView : public Subscriber<SenseControllerEvent> {

protected:
	virtual RESULT Notify(SenseControllerEvent *pEvent) = 0;

protected:
	float m_velocity = 0.0f;
	double m_frameMs;

};

#endif // ! UI_SCROLL_VIEW_H_