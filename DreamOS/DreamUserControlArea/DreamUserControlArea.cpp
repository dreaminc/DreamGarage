#include "DreamUserControlArea.h"

#include "DreamOS.h"
#include "DreamUserApp.h"
#include "DreamGarage/DreamBrowser.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"	

DreamUserControlArea::DreamUserControlArea(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUserControlArea>(pDreamOS, pContext)
{
	// empty
}

DreamUserControlArea::~DreamUserControlArea() 
{

}

RESULT DreamUserControlArea::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	//m_pDreamUserApp = GetDOS()->LaunchDreamApp<DreamUserApp>(this);
	//CN(m_pDreamUserApp);
	m_aspectRatio = ((float)1366 / (float)768);
	m_castWidth = std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	m_castHeight = std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));

	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());

Error:
	return r;
}

RESULT DreamUserControlArea::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamUserControlArea::Update(void *pContext) {
	RESULT r = R_PASS;

	//CR(m_pWebBrowserManager->Update());

//Error:
	return r;
}

RESULT DreamUserControlArea::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	//CR(m_pWebBrowserManager->Shutdown());

//Error:
	return r;
}

DreamUserControlArea* DreamUserControlArea::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUserControlArea *pDreamApp = new DreamUserControlArea(pDreamOS, pContext);
	return pDreamApp;
}

float DreamUserControlArea::GetBaseWidth() {
	return m_baseWidth;
}

float DreamUserControlArea::GetBaseHeight() {
	return m_baseHeight;
}

float DreamUserControlArea::GetSpacingSize() {
	return m_spacingSize;
}

RESULT DreamUserControlArea::HandleControlBarEvent(ControlEventType type) {
	RESULT r = R_PASS;

	switch (type) {

	case ControlEventType::BACK: {
		// Send back event to active browser
	} break;

	case ControlEventType::FORWARD: {
		// Send forward event to active browser
	} break;

	case ControlEventType::OPEN: {
		// pull up menu to select new piece of content
		// send hide events to control bar, control view, and tab bar
	} break;

	case ControlEventType::CLOSE: {
		// if active browser matches shared browser, send stop event
		// close active browser
		// replace with top of tab bar
		// update tab bar
	} break;

	case ControlEventType::MAXIMIZE: {
		// send show events to control view and tab bar
	} break;

	case ControlEventType::MINIMIZE: {
		// send hide events to control view and tab bar
	} break;

	case ControlEventType::SHARE: {
		// send share event with active browser
	} break;

	case ControlEventType::STOP: {
		// send stop sharing event 
	} break;

	case ControlEventType::URL: {
		// dismiss everyting(?) and pull up the keyboard
	}

	}

	return r;
}