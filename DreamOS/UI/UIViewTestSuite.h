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
	RESULT AddTestKeyboardAngle();

	RESULT UpdateTextBox(std::string strEntered);

	virtual RESULT AddTests() override;

// Shared functions
public:
	RESULT DefaultCallback(void *pContext);
	RESULT ResetTestCallback(void *pContext);
	RESULT UpdateHandRay(void *pContext);
	RESULT IncreaseAngle(void *pContext);
	RESULT DecreaseAngle(void *pContext);

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

	float m_lineHeight = TEXTBOX_LINE_HEIGHT;
	float m_numLines = TEXTBOX_NUM_LINES;
	float m_lineWidth = TEXTBOX_WIDTH;
	std::shared_ptr<text> m_pTextBoxText;
	std::shared_ptr<quad> m_pTextBoxBackground;
	std::shared_ptr<composite> m_tComposite;
	std::shared_ptr<font> m_pFont;
	std::shared_ptr<texture> m_pTextBoxTexture;
};

#define UI_VIEW_TEST_SUITE_H_
#endif // ! UI_VIEW_TEST_SUITE_H_
