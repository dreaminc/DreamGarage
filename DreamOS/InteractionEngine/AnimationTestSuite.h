#ifndef ANIMATION_TEST_SUITE_H_
#define ANIMATION_TEST_SUITE_H_

#include "RESULT/EHM.h"
#include "Test/TestSuite.h"

#include "Primitives/Subscriber.h"
#include "Sense/SenseKeyboard.h"

#include <memory>
#include <chrono>

class DreamOS;
class composite;
class sphere; 
class InteractionEngine;
struct InteractionObjectEvent;

struct AnimationTestContext {
	composite *pComposite = nullptr;
	sphere* pSphere = nullptr;
	sphere* pSphere2 = nullptr;
	bool fCancelled;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
};

class AnimationTestSuite : public TestSuite, public Subscriber<InteractionObjectEvent>, public Subscriber<SenseKeyboardEvent> {
public:
	AnimationTestSuite(DreamOS *pDreamOS);
	~AnimationTestSuite();

	virtual RESULT AddTests() override;

public:
	RESULT ResetTest(void *pContext);
	RESULT AddTestAnimationBasic();
	RESULT AddTestCancel();
	RESULT AddTestCurves();
	RESULT AddTestRotate();
	RESULT AddTestUIColor();

	RESULT SetupPipeline();
	RESULT SetupUINodePipeline();

private:
	RESULT InitializeAnimationTest(void *pContext);

	virtual RESULT Notify(InteractionObjectEvent *event) override;
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;

	DreamOS *m_pDreamOS;
};

#endif // ! ANIMATION_TEST_SUITE_H_