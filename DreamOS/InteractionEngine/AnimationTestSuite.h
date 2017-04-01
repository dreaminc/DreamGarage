#ifndef ANIMATION_TEST_SUITE_H_
#define ANIMATION_TEST_SUITE_H_

#include "RESULT/EHM.h"
#include "Test/TestSuite.h"

#include "Primitives/Subscriber.h"
#include "InteractionEngine/InteractionEngine.h"
#include "InteractionEngine/InteractionObjectEvent.h"
#include "Sense/SenseKeyboard.h"

class DreamOS;

class AnimationTestSuite : public TestSuite, public Subscriber<InteractionObjectEvent>, public Subscriber<SenseKeyboardEvent> {
public:
	AnimationTestSuite(DreamOS *pDreamOS);
	~AnimationTestSuite();

	virtual RESULT AddTests() override;

public:
	RESULT AddTestAnimation();

private:
	virtual RESULT Notify(InteractionObjectEvent *event) override;
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;

	DreamOS *m_pDreamOS;
};

#endif // ! ANIMATION_TEST_SUITE_H_