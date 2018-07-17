#ifndef DREAM_FORM_APP_H_
#define DREAM_FORM_APP_H_

#include "DreamApp.h"
#include "DreamOS.h"

#include "DreamGarage/DreamBrowser.h"

class DreamControlView;
struct UIEvent;

class DreamFormApp : public DreamApp<DreamFormApp>,
					public DreamBrowserObserver,
					public Subscriber<UIEvent>, 
					public Subscriber<InteractionObjectEvent>
{
	friend class DreamAppManager;
	friend class MultiContentTestSuite;

public:
	DreamFormApp(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamFormApp();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr);
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr);
	virtual RESULT Update(void *pContext = nullptr);
	virtual RESULT Shutdown(void *pContext = nullptr);

protected:
	static DreamFormApp* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

public:
	RESULT UpdateWithNewForm(std::string strURL);

// Dream Browser observer
public:
	RESULT HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) override;

	RESULT UpdateControlBarText(std::string& strTitle) override;
	RESULT UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) override;

	RESULT UpdateContentSourceTexture(std::shared_ptr<texture> pTexture, DreamContentSource *pContext) override;

	RESULT HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) override;
	RESULT HandleIsInputFocused(bool fIsFocused, DreamContentSource *pContext) override;

	RESULT HandleDreamFormSuccess() override;

	RESULT HandleCanTabNext(bool fCanNext) override;
	RESULT HandleCanTabPrevious(bool fCanPrevious) override;

private:
	WebBrowserPoint GetRelativePointofContact(point ptContact);

public:
	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;
	virtual RESULT Notify(UIEvent *pUIEvent) override;

public:
	RESULT Show();
	RESULT Hide();

protected:
	std::shared_ptr<DreamBrowser> m_pDreamBrowserForm = nullptr;
	std::shared_ptr<DreamControlView> m_pFormView = nullptr;
	DreamUserApp* m_pUserApp = nullptr;

//private:
protected:
	std::string m_strURL;
	bool m_fInitBrowser = false;

};

#endif // ! DREAM_FORM_APP_H_
