#ifndef DREAM_CONTROL_BAR_H_
#define DREAM_CONTROL_BAR_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"

#include "UI/UIView.h"
#include "DreamControlView/UIControlBar.h"

#include "DreamGarage/UICommon.h"

//#include "DreamUserControlArea/DreamUserControlArea.h"
class DreamUserControlArea;

//All relative to parent app
#define URL_WIDTH 0.5484

class UIContentControlBar : public UIControlBar {

public:
	UIContentControlBar(HALImp *pHALImp, DreamOS *pDreamOS);
	~UIContentControlBar();

	// DreamApp
public:
	RESULT Initialize(DreamUserControlArea *pParent);
	RESULT Update();

	// ControlBarObserver
public:
	RESULT HandleBackPressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleForwardPressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleShowTogglePressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleOpenPressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleClosePressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleShareTogglePressed(UIButton *pButtonContext, void *pContext);
	RESULT HandleURLPressed(UIButton* pButtonContext, void* pContext);
	RESULT HandleKeyboardPressed(UIButton* pButtonContext, void* pContext);

	// Also updates the button texture
	RESULT SetSharingFlag(bool fIsSharing);
	RESULT SetTitleText(const std::string& strTitle);
	RESULT UpdateControlBarButtonsWithType(std::string strContentType);

	RESULT ClearMinimizedState();

// Animations
public:
	RESULT Show();
	RESULT Hide();

private:

	DreamUserControlArea* m_pParentApp = nullptr;

	bool m_fIsMinimized = false;
	bool m_fIsSharing = false;

	bool m_fUpdateTitle = false;
	std::string m_strUpdateTitle = "";
	BarType m_barType = BarType::DEFAULT;

	double m_buttonWidth = ITEM_SIZE;
	double m_buttonHeight = ITEM_SIZE;
	double m_urlWidth = URL_WIDTH;
	double m_urlHeight = ITEM_SIZE;
};

#endif // ! DREAM_CONTROL_BAR_H_