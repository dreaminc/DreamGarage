#ifndef DREAM_USER_CONTROL_AREA_H_
#define DREAM_USER_CONTROL_AREA_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"
#include "DreamGarage/DreamControlBar.h"

#include <vector>
#include <map>

class DreamUserApp;

class CEFBrowserManager;

class quad;

#define MAIN_DIAGONAL 0.6f
#define SPACING_SIZE 0.016129f
#define DEFAULT_PX_WIDTH 1366
#define DEFAULT_PX_HEIGHT 768
 
class DreamUserControlArea : public DreamApp<DreamUserControlArea> {
	friend class DreamAppManager;
	friend class MultiContentTestSuite;

public:
	DreamUserControlArea(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamUserControlArea();

// DreamApp
public:
	virtual RESULT InitializeApp(void *pContext) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamUserControlArea* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

// Expose size variables
public:
	float GetSpacingSize();
	float GetBaseWidth();
	float GetBaseHeight();

// For use with DreamControlBar
public:
	RESULT HandleControlBarEvent(ControlEventType type);

	RESULT CanPressButton(UIButton *pButtonContext);

private:
	std::shared_ptr<DreamUserApp> m_pDreamUserApp;
	std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;

	std::shared_ptr<DreamControlBar> m_pControlBar;

	float m_spacingSize = SPACING_SIZE;
	float m_pxWidth = DEFAULT_PX_WIDTH;
	float m_pxHeight = DEFAULT_PX_HEIGHT;

	float m_diagonalSize = MAIN_DIAGONAL;
	float m_aspectRatio;
	float m_baseWidth;
	float m_baseHeight;
};

#endif // ! DREAM_USER_CONTROL_AREA_H_