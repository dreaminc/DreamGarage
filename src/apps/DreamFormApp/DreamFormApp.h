#ifndef DREAM_FORM_APP_H_
#define DREAM_FORM_APP_H_

#include "core/ehm/EHM.h"

// Dream Form App
// dos/src/app/DreamFormApp/DreamFormApp.h

#include "app/DreamApp.h"
#include "os/DreamOS.h"

// TODO: This should not be here
#include "apps/DreamBrowserApp/DreamBrowserApp.h"

class UIControlView;
struct UIEvent;
struct HMDEvent;

enum class FormType {
	SIGN_IN,
	SIGN_UP,
	SIGN_UP_WELCOME,
	ENVIRONMENTS_WELCOME,
	SETTINGS,
	TEAMS_MISSING,
	CERTIFICATE_ERROR,
	LOAD_RESOURCE_ERROR,
	DEFAULT,
	INVALID
};

class DreamFormApp : public DreamApp<DreamFormApp>,
					public DreamBrowserObserver,
					public Subscriber<UIEvent>, 
					public Subscriber<InteractionObjectEvent>,
					public Subscriber<HMDEvent>
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

// form key string/type converters
public:
	static std::string StringFromType(FormType type);
	static FormType TypeFromString(std::string& strType);

	virtual std::string GetSuccessString();
	FormType GetFormType();

public:
	RESULT UpdateWithNewForm(std::string strURL);
	RESULT ResetForm();

// Dream Browser observer
public:
	RESULT HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) override;

	RESULT UpdateControlBarText(std::string& strTitle) override;
	RESULT UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) override;
	RESULT UpdateAddressBarSecurity(bool fSecure) override;
	RESULT UpdateAddressBarText(std::string& strText) override;

	RESULT UpdateContentSourceTexture(texture* pTexture, std::shared_ptr<DreamContentSource> pContext) override;

	RESULT HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) override;
	RESULT HandleIsInputFocused(bool fIsFocused, DreamContentSource *pContext) override;
	RESULT HandleLoadEnd() override;

	RESULT HandleDreamFormSuccess() override;
	RESULT HandleDreamFormCancel() override;
	RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& accessToken) override { return R_NOT_IMPLEMENTED; };
	RESULT HandleDreamFormSetEnvironmentId(int environmentId) override { return R_NOT_IMPLEMENTED; };

	RESULT HandleCanTabNext(bool fCanNext) override;
	RESULT HandleCanTabPrevious(bool fCanPrevious) override;

	virtual std::string GetCertificateErrorURL() override;
	virtual std::string GetLoadErrorURL() override;

	RESULT SetAsActive();

	RESULT SetFormType(FormType type);

private:
	WebBrowserPoint GetRelativePointofContact(point ptContact);

public:
	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;
	virtual RESULT Notify(UIEvent *pUIEvent) override;
	virtual RESULT Notify(HMDEvent *pHMDEvent) override;

public:
	RESULT Show();
	RESULT Hide();

protected:
	std::shared_ptr<DreamBrowser> m_pDreamBrowserForm = nullptr;
	std::shared_ptr<UIControlView> m_pFormView = nullptr;

//private:
protected:
	std::string m_strURL;
	std::string m_strSuccess = "DreamFormApp.OnSuccess";

	bool m_fInitBrowser = false;
	bool m_fUpdateFormURL = false;
	bool m_fBrowserFinishedLoading = false;

	bool m_fTyping = false;
	bool m_fFormVisible = false;
	bool m_fPendShowFormView = false;
	bool m_fSetAsActive = false;

	bool m_fPendHMDRecenter = false;

	FormType m_formType;
};

#endif // ! DREAM_FORM_APP_H_
