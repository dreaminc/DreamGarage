#ifndef UI_VIEW_TEST_SUITE_H_



#include "RESULT/EHM.h"

#include "Primitives/valid.h"
#include "Primitives/TextEntryString.h"
#include "Primitives/Subscriber.h"
#include "Test/TestSuite.h"


#define TEXTBOX_LINE_HEIGHT 0.027f // text box records what has been typed
#define TEXTBOX_NUM_LINES 1.0f
#define TEXTBOX_WIDTH 0.2f

class DreamOS;
class UIView;
class UIButton;
struct UIEvent;
struct SenseControllerEvent;

class UIViewTestSuite : public valid, public TestSuite, public Subscriber<UIEvent>, public Subscriber<SenseControllerEvent>
{
public:
	UIViewTestSuite(DreamOS *pDreamOS);
	~UIViewTestSuite();

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
	RESULT AddTestUIScrollView();
	RESULT AddTestDreamUIBar();
	RESULT AddTestKeyboardAngle();

	RESULT UpdateTextBox(std::string strEntered);

	virtual RESULT AddTests() override;

// Shared functions
public:
	RESULT DefaultCallback(void *pContext);
	RESULT ResetTestCallback(void *pContext);
	RESULT UpdateHandRay(void *pContext);
	RESULT IncreaseAngleButton(void *pContext);
	RESULT DecreaseAngleButton(void *pContext);
	RESULT IncreaseAngleTrigger();
	RESULT DecreaseAngleTrigger();

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
	virtual RESULT Notify(SenseControllerEvent *event) override;

private:
	DreamOS *m_pDreamOS;
	float m_malletAngle = 180.0f;
};

#define UI_VIEW_TEST_SUITE_H_
#endif // ! UI_VIEW_TEST_SUITE_H_
