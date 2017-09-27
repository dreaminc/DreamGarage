#ifndef UI_VIEW_TEST_SUITE_H_

#include "RESULT/EHM.h"

#include "Primitives/valid.h"
#include "Primitives/Subscriber.h"
#include "Test/TestSuite.h"

class DreamOS;
class UIView;
class UIButton;
struct UIEvent;

class UIViewTestSuite : public valid, public TestSuite, public Subscriber<UIEvent>
{
public:
	UIViewTestSuite(DreamOS *pDreamOS);
	~UIViewTestSuite();

	RESULT Initialize();

private:
	RESULT SetupPipeline();
	RESULT SetupUINodePipeline();
	RESULT SetupUIStagePipeline();

// Tests
public:
	RESULT AddTestUIView();
	RESULT AddTestUIButton();
	RESULT AddTestUIButtons();
	RESULT AddTestUIScrollView();
	RESULT AddTestDreamUIBar();

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
	DreamOS *m_pDreamOS;
};

#define UI_VIEW_TEST_SUITE_H_
#endif // ! UI_VIEW_TEST_SUITE_H_