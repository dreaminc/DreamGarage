#ifndef DREAM_CONTROL_BAR_H_
#define DREAM_CONTROL_BAR_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"

#include "UI/UIView.h"
#include "DreamControlView/UIControlBar.h"
//#include "DreamUserControlArea/DreamUserControlArea.h"
class DreamUserControlArea;

//All relative to parent app
#define BUTTON_WIDTH 0.0645
#define BUTTON_HEIGHT 0.0645
#define URL_WIDTH 0.5484
#define URL_HEIGHT 0.0645

enum class ControlEventType {
	OPEN,
	CLOSE,
	SHARE,
	STOP,
	BACK,
	FORWARD,
	MAXIMIZE,
	MINIMIZE,
	URL,
	INVALID
};

class DreamControlBar : public DreamApp<DreamControlBar>, public ControlBarObserver {
	friend class DreamAppManager;
	friend class DreamUserControlArea; // could be temporary, needs to access composite

public:
	DreamControlBar(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamControlBar();

// DreamApp
public:
	virtual RESULT InitializeApp(void *pContext) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamControlBar* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

// ControlBarObserver
public:
	RESULT HandleBackPressed(UIButton* pButtonContext, void* pContext) override;
	RESULT HandleForwardPressed(UIButton* pButtonContext, void* pContext) override;
	RESULT HandleShowTogglePressed(UIButton* pButtonContext, void* pContext) override;
	RESULT HandleOpenPressed(UIButton* pButtonContext, void* pContext) override;
	RESULT HandleClosePressed(UIButton* pButtonContext, void* pContext) override;
	RESULT HandleShareTogglePressed(UIButton *pButtonContext, void *pContext) override;
	RESULT HandleURLPressed(UIButton* pButtonContext, void* pContext) override;

	bool CanPressButton(int dirtyIndex);
	RESULT ClearMalletFlag(int index);

	// Also updates the button texture
	RESULT SetSharingFlag(bool fIsSharing);

public:
	RESULT InitializeWithParent(DreamUserControlArea *pParentApp);

private:

	std::shared_ptr<UIView> m_pView = nullptr;
	std::shared_ptr<UIControlBar> m_pUIControlBar = nullptr;

	DreamUserControlArea* m_pParentApp = nullptr;

	bool m_fIsMinimized = false;
	bool m_fIsSharing = false;

	double m_buttonWidth = BUTTON_WIDTH;
	double m_buttonHeight = BUTTON_HEIGHT;
	double m_urlWidth = URL_WIDTH;
	double m_urlHeight = URL_HEIGHT;

	bool m_fCanPressButton[2];
};

#endif // ! DREAM_CONTROL_BAR_H_