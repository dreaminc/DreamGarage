#ifndef UI_VIEW_TEST_SUITE_H_
#define UI_VIEW_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// TODO: Merge with the UI Test Suite
// Dream UI View Test Suite
// dos/src/test/suites/UIViewTestSuite.h

#include "DreamTestSuite.h"

#include "core/types/Subscriber.h"

class DreamOS;
class UIView;
class UIButton;
class UIStageProgram;
struct UIEvent;

class UIViewTestSuite : public DreamTestSuite, public Subscriber<UIEvent>
{
public:
	UIViewTestSuite(DreamOS *pDreamOS);
	~UIViewTestSuite() = default;

	RESULT Initialize();

private:
	RESULT SetupPipeline();
	RESULT SetupUINodePipeline();
	RESULT SetupUIStagePipeline(UIStageProgram* &pUIStageProgram);

// Tests
public:
	RESULT AddTestUIView();
	RESULT AddTestUIButton();
	RESULT AddTestUIButtons();
	RESULT AddTestUISpatialScrollView();
	RESULT AddTestDreamUIBar();
	RESULT AddTestKeyboardAngle();
	RESULT AddTestCurvedTitle();
	RESULT AddTestDreamControlView();
	RESULT AddTestDreamBaseUI();

	virtual RESULT AddTests() override;

// Shared functions
public:
	RESULT DefaultCallback(void *pContext);
	RESULT ResetTestCallback(void *pContext);
	RESULT UpdateHandRay(void *pContext);

// behaviors
public:
	RESULT Rotate45(UIButton *pButtonContext, void *pContext);
	RESULT Rotate15(UIButton *pButtonContext, void *pContext);
	RESULT ResetRotation(UIButton *pButtonContext, void *pContext);

	RESULT AnimateScaleUp(UIButton *pButtonContext, void *pContext);
	RESULT AnimateScaleReset(UIButton *pButtonContext, void *pContext);

	RESULT AnimateMoveUpAndBack(UIButton *pButtonContext, void *pContext);

public:
	virtual RESULT Notify(UIEvent *pEvent) override;

private:
	DreamOS *m_pDreamOS = nullptr;
};

#endif // ! UI_VIEW_TEST_SUITE_H_
