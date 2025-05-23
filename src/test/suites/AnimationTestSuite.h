#ifndef ANIMATION_TEST_SUITE_H_
#define ANIMATION_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// TODO: Move to tests
// Dream Animation Test Suite
// dos/src/modules/AnimationEngine/AnimationTestSuite.h

#include <memory>
#include <chrono>

#include "DreamTestSuite.h"

#include "core/types/Subscriber.h"

#include "sense/SenseKeyboard.h"

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

class AnimationTestSuite : 
	public DreamTestSuite, 
	public Subscriber<InteractionObjectEvent>, 
	public Subscriber<SenseKeyboardEvent> 
{
public:
	AnimationTestSuite(DreamOS *pDreamOS);
	~AnimationTestSuite() = default;

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
};

#endif // ! ANIMATION_TEST_SUITE_H_