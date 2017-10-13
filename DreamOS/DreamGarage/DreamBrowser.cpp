#include "DreamBrowser.h"
#include "DreamOS.h"
#include "Core/Utilities.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "InteractionEngine/AnimationItem.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"
#include "WebBrowser/WebBrowserController.h"

#include "Cloud/Environment/EnvironmentAsset.h"

#include "Cloud/WebRequest.h"

RESULT DreamBrowserHandle::SetScope(std::string strScope) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SetBrowserScope(strScope));
Error:
	return r;
}

RESULT DreamBrowserHandle::SetScrollingParams(WebBrowserPoint ptDiff) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ScrollBrowser(ptDiff));
Error:
	return r;
}

RESULT DreamBrowserHandle::SetPath(std::string strPath) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SetBrowserPath(strPath));
Error:
	return r;
}

RESULT DreamBrowserHandle::SetClickParams(WebBrowserPoint ptContact) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ClickBrowser(ptContact));
Error:
	return r;
}

std::shared_ptr<texture> DreamBrowserHandle::GetBrowserTexture() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return BrowserTexture();
Error:
	return nullptr;
}

DreamBrowser::DreamBrowser(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamBrowser>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamBrowser::~DreamBrowser(){
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

RESULT DreamBrowser::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	if (m_pWebBrowserManager != nullptr) {
		CR(m_pWebBrowserManager->Shutdown());
		//m_pWebBrowserManager = nullptr;
	}

Error:
	return r;
}

DreamAppHandle* DreamBrowser::GetAppHandle() {
	return (DreamBrowserHandle*)(this);
}

RESULT DreamBrowser::ScrollBrowser(WebBrowserPoint ptDiff) {
	RESULT r = R_PASS;
	WebBrowserMouseEvent mouseEvent;

	mouseEvent.pt = m_lastWebBrowserPoint;	//maybe we don't need this, that'd be nice

	int deltaX = 0;	//ptDiff.x
	int deltaY = ptDiff.y;

	CR(m_pWebBrowserController->SendMouseWheel(mouseEvent, deltaX, deltaY));
Error:
	return r;
}

RESULT DreamBrowser::ClickBrowser(WebBrowserPoint ptContact) {
	RESULT r = R_PASS;
	WebBrowserMouseEvent mouseEvent;

	mouseEvent.pt = ptContact;

	mouseEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;
	CR(m_pWebBrowserController->SendMouseClick(mouseEvent, false, 1));	// mouse down
	CR(m_pWebBrowserController->SendMouseClick(mouseEvent, true, 1));		// mouse up
Error:
	return r;
}

std::shared_ptr<texture> DreamBrowser::BrowserTexture() {
	return m_pBrowserTexture;
}

// TODO: Only update the rect
RESULT DreamBrowser::OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

	CN(m_pBrowserTexture);
	//CR(m_pBrowserTexture->Update((unsigned char*)(pBuffer), width, height, texture::PixelFormat::BGRA));
	CR(m_pBrowserTexture->Update((unsigned char*)(pBuffer), width, height, texture::PixelFormat::BGRA));
	
Error:
	return r;
}

RESULT DreamBrowser::OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamBrowser::FadeQuadToBlack() {

	RESULT r = R_PASS;

	//Fade to black
	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		//m_pBrowserQuad->SetVisible(false);
		return r;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pBrowserQuad.get(),
		color(0.0f, 0.0f, 0.0f, 1.0f),
		0.1f,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamBrowser::OnLoadStart() {
	RESULT r = R_PASS;

	return r;
}

RESULT DreamBrowser::OnLoadEnd(int httpStatusCode) {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		//m_pBrowserQuad->SetVisible(true);
		return r;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pBrowserQuad.get(),
		color(1.0f, 1.0f, 1.0f, 1.0f),
		0.1f,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		fnStartCallback,
		nullptr,
		this
	));

Error:
	return r;
}

// DreamApp Interface
RESULT DreamBrowser::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	std::string strURL = "http://www.google.com";

	int pxWidth = 1366;
	int pxHeight = 768;
	m_aspectRatio = ((float)pxWidth / (float)pxHeight);
	std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

	// Subscribers (children)
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(GetDOS()->RegisterEventSubscriber(GetComposite(), (InteractionEventType)(i), this));
	}

	// Controller
	//RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_MENU_UP, this);
	//GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_TRIGGER_UP, this);
	//GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_TRIGGER_DOWN, this);

	SetAppName("DreamBrowser");
	SetAppDescription("A Shared Content View");

	// Set up browser manager
	m_pWebBrowserManager = std::make_shared<CEFBrowserManager>();
	CN(m_pWebBrowserManager);
	CR(m_pWebBrowserManager->Initialize());

	// Initialize new browser
	m_pWebBrowserController = m_pWebBrowserManager->CreateNewBrowser(pxWidth, pxHeight, strURL);
	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->RegisterWebBrowserControllerObserver(this));

	// Set up the quad
	SetNormalVector(vector(0.0f, 1.0f, 0.0f).Normal());
	m_pBrowserQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());

	/*
	m_pTestSphereAbsolute = GetDOS()->AddSphere(0.025f, 10, 10);
	m_pTestSphereAbsolute->SetColor(COLOR_RED);

	m_pTestSphereRelative = GetComposite()->AddSphere(0.025f, 10, 10);
	m_pTestSphereRelative->SetColor(COLOR_RED);
	*/
	
	// Flip UV vertically
	///*
	m_pBrowserQuad->FlipUVVertical();
	//*/

	m_pBrowserQuad->SetMaterialAmbient(0.8f);

	// Set up and map the texture
	m_pBrowserTexture = GetComposite()->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, pxWidth, pxHeight, texture::PixelFormat::RGBA, 4, &vectorByteBuffer[0], pxWidth * pxHeight * 4);	
	
	m_pBrowserQuad->SetDiffuseTexture(m_pBrowserTexture.get());

	// Set up mouse / hand cursor model
	/*
	m_pPointerCursor = GetComposite()->AddModel(L"\\Models\\mouse-cursor\\mouse-cursor.obj",
												nullptr,
												point(-0.2f, -0.43f, 0.0f),
												0.01f,
												vector(-(float)M_PI_2, 0.0f, 0.0f));
	m_pPointerCursor->SetMaterialAmbient(1.0f);
	m_pPointerCursor->SetVisible(false);
	//*/

	/*
	m_pHandCursor = GetComposite()->AddModel(L"\\Models\\mouse-hand\\mouse-hand.obj",
											 nullptr,
											 point(0.0f, 0.0f, 0.0f),
											 0.002f,
											 vector(0.0f, 0.0f, 0.0f));
	//*/

	GetDOS()->AddObjectToInteractionGraph(m_pBrowserQuad.get());

Error:
	return r;
}

RESULT DreamBrowser::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamBrowser::Update(void *pContext) {
	RESULT r = R_PASS;

	ray rCast;

	if (m_pWebBrowserManager != nullptr) {
		CR(m_pWebBrowserManager->Update());
	}
	else {
		SetVisible(false);
	}

	//CR(GetDOS()->UpdateInteractionPrimitive(GetHandRay()));

	/*
	{
		CollisionManifold manifold = m_pBrowserQuad->Collide(rCast);

		if (manifold.NumContacts() > 0) {
			m_pPointerCursor->SetOrigin(manifold.GetContactPoint(0).GetPoint());
		}
	}
	*/

Error:
	return r;
}

WebBrowserPoint DreamBrowser::GetRelativeBrowserPointFromContact(point ptIntersectionContact) {
	WebBrowserPoint webPt;

	ptIntersectionContact.w() = 1.0f;

	// First apply transforms to the ptIntersectionContact 
	point ptAdjustedContact = inverse(m_pBrowserQuad->GetModelMatrix()) * ptIntersectionContact;
	
	//m_pTestSphereRelative->SetPosition(ptAdjustedContact);

	float width = GetWidth();
	float height = GetHeight();

	float posX = ptAdjustedContact.x();
	float posY = ptAdjustedContact.y();
	float posZ = ptAdjustedContact.z();

	// TODO: This is a bit of a hack, should be a better way (this won't account for the quad normal, only orientation
	// so it might get confused - technically this should never actually happen otherwise since we can force a dimension
	if (std::abs(posZ) > std::abs(posY)) {
		posY = posZ;
	}

	posX /= width / 2.0f;
	posY /= height / 2.0f;

	posX = (posX + 1.0f) / 2.0f;
	posY = (posY + 1.0f) / 2.0f;  // flip it

	// TODO: push into WebBrowserController
	webPt.x = posX * 1366;
	webPt.y = 768 - (posY * 768);

	//ptAdjustedContact.Print("adj");
	//DEBUG_LINEOUT("%d %d", webPt.x, webPt.y);

	return webPt;
}

// InteractionObjectEvent
RESULT DreamBrowser::Notify(InteractionObjectEvent *pEvent) {
	RESULT r = R_PASS;

	bool fUpdateMouse = false;

	//m_pTestSphereAbsolute->SetPosition(pEvent->m_ptContact[0]);
	switch (pEvent->m_eventType) {
	/*
		case ELEMENT_INTERSECT_BEGAN: {
			if (m_pBrowserQuad->IsVisible()) {
				m_pPointerCursor->SetVisible(true);

				WebBrowserMouseEvent webBrowserMouseEvent;

				webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

				CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, false));

				m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
				m_fBrowserActive = true;

				fUpdateMouse = true;
			}
		} break;

		case ELEMENT_INTERSECT_ENDED: {
			m_pPointerCursor->SetVisible(false);

			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

			CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, true));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
			m_fBrowserActive = false;

			fUpdateMouse = true;
		} break;

		case ELEMENT_INTERSECT_MOVED: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

			CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, false));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;

			fUpdateMouse = true;
		} break;

		case INTERACTION_EVENT_SELECT_UP: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			bool fMouseUp = (pEvent->m_eventType == INTERACTION_EVENT_SELECT_UP);

			webBrowserMouseEvent.pt = m_lastWebBrowserPoint;
			webBrowserMouseEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;

			CR(m_pWebBrowserController->SendMouseClick(webBrowserMouseEvent, fMouseUp, 1));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
		} break;

		case INTERACTION_EVENT_SELECT_DOWN: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			bool fMouseUp = (pEvent->m_eventType == INTERACTION_EVENT_SELECT_UP);

			webBrowserMouseEvent.pt = m_lastWebBrowserPoint;
			webBrowserMouseEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;

			CR(m_pWebBrowserController->SendMouseClick(webBrowserMouseEvent, fMouseUp, 1));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
		} break;

		case INTERACTION_EVENT_WHEEL: {
			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = m_lastWebBrowserPoint;
			
			int deltaX = 0;
			int deltaY = pEvent->m_value * m_scrollFactor;

			CR(m_pWebBrowserController->SendMouseWheel(webBrowserMouseEvent, deltaX, deltaY));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
		} break;
		//*/
		// Keyboard
		// TODO: Should be a "typing manager" in between?
		// TODO: haven't seen any issues with KEY_UP being a no-op
		case INTERACTION_EVENT_KEY_UP: break;
		case INTERACTION_EVENT_KEY_DOWN: {
			bool fKeyDown = (pEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN);
			std::string strURL = m_strEntered.GetString();

			char chKey = (char)(pEvent->m_value);
			m_strEntered.UpdateString(chKey);
			
			if (pEvent->m_value == SVK_RETURN) {
				SetVisible(true);

				std::string strScope = m_strScope;
				auto keyUIDs = GetDOS()->GetAppUID("UIKeyboard");
				
				CB(keyUIDs.size() == 1);
				{
					UID keyUID = keyUIDs[0];
					auto pKeyboardHandle = dynamic_cast<UIKeyboardHandle*>(GetDOS()->CaptureApp(keyUID, this));
					CN(pKeyboardHandle);

					pKeyboardHandle->Hide();
					CR(GetDOS()->ReleaseApp(pKeyboardHandle, keyUID, this));

				}

				std::string strTitle = "website";
				std::string strPath = strURL;
				auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
				CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

				CRM(m_pEnvironmentControllerProxy->RequestShareAsset(m_strScope, strPath, strTitle), "Failed to share environment asset");

			}

			//CR(m_pWebBrowserController->SendKeyEventChar(chKey, fKeyDown));

		} break;
	}
/*
	// First point of contact
	if (fUpdateMouse) {
		//if (pEvent->m_ptContact[0] != GetDOS()->GetInteractionEngineProxy()->GetInteractionRayOrigin()) {
			//m_pPointerCursor->SetOrigin(pEvent->m_ptContact[0]);
			point ptIntersectionContact = pEvent->m_ptContact[0];
			ptIntersectionContact.w() = 1.0f;

			point ptAdjustedContact = inverse(m_pBrowserQuad->GetModelMatrix()) * ptIntersectionContact;
			m_pPointerCursor->SetOrigin(ptAdjustedContact);
		//}
	}
	//*/

Error:
	return r;
}

RESULT DreamBrowser::SetPosition(point ptPosition) {
	GetComposite()->SetPosition(ptPosition);
	return R_PASS;
}

RESULT DreamBrowser::SetAspectRatio(float aspectRatio) {
	m_aspectRatio = aspectRatio;

	if (m_pBrowserQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamBrowser::SetDiagonalSize(float diagonalSize) {
	m_diagonalSize = diagonalSize;

	if (m_pBrowserQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamBrowser::SetNormalVector(vector vNormal) {
	m_vNormal = vNormal.Normal();

	if (m_pBrowserQuad != nullptr) {
		return UpdateViewQuad();
	}

	return R_PASS;
}

RESULT DreamBrowser::SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal) {
	GetComposite()->SetPosition(ptPosition);
	m_diagonalSize = diagonal;
	m_aspectRatio = aspectRatio;
	m_vNormal = vNormal.Normal();

	if (m_pBrowserQuad != nullptr) {
		return UpdateViewQuad();
	}

	return R_PASS;
}

float DreamBrowser::GetAspectRatio() {
	return m_aspectRatio;
}

float DreamBrowser::GetHeight() {
	return std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

float DreamBrowser::GetWidth() {
	return std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

vector DreamBrowser::GetNormal() {
	return m_vNormal;
}

point DreamBrowser::GetOrigin() {
	return GetComposite()->GetOrigin();
}

RESULT DreamBrowser::UpdateViewQuad() {
	RESULT r = R_PASS;

	CR(m_pBrowserQuad->UpdateParams(GetWidth(), GetHeight(), GetNormal()));
	
	// Flip UV vertically
	///*
	if (r != R_SKIPPED) {
		m_pBrowserQuad->FlipUVVertical();
	}
	//*/

	CR(m_pBrowserQuad->SetDirty());

	CR(m_pBrowserQuad->InitializeBoundingQuad(point(0.0f, 0.0f, 0.0f), GetWidth(), GetHeight(), GetNormal()));

Error:
	return r;
}

bool DreamBrowser::IsVisible() {
	return m_pBrowserQuad->IsVisible();
}

RESULT DreamBrowser::SetVisible(bool fVisible) {
	RESULT r = R_PASS;
	CR(m_pBrowserQuad->SetVisible(fVisible));
	//CR(m_pPointerCursor->SetVisible(fVisible));
Error:
	return r;
}

RESULT DreamBrowser::SetBrowserScope(std::string strScope) {
	m_strScope = strScope;
	return R_PASS;
}

RESULT DreamBrowser::SetBrowserPath(std::string strPath) {
	m_strPath = strPath;
	return R_PASS;
}

RESULT DreamBrowser::SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	if (pEnvironmentAsset != nullptr) {
		WebRequest webRequest;

		std::string strEnvironmentAssetURI = pEnvironmentAsset->GetURI();

		std::wstring wstrAssetURI = util::StringToWideString(strEnvironmentAssetURI);
		CR(webRequest.SetURL(wstrAssetURI));
		CR(webRequest.SetRequestMethod(WebRequest::Method::GET));
	
		UserControllerProxy *pUserControllerProxy = (UserControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
		CN(pUserControllerProxy);

		std::string strUserToken = pUserControllerProxy->GetUserToken();
		std::wstring wstrUserToken = util::StringToWideString(strUserToken);
		CR(webRequest.AddRequestHeader(L"Authorization", L"Token " + wstrUserToken));

		LoadRequest(webRequest);
	}

Error:
	return r;
}

RESULT DreamBrowser::SetURI(std::string strURI) {
	RESULT r = R_PASS;

	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->LoadURL(strURI));

Error:
	return r;
}

RESULT DreamBrowser::LoadRequest(const WebRequest &webRequest) {
	RESULT r = R_PASS;

	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->LoadRequest(webRequest));

Error:
	return r;
}

// TODO: Remove
/*
RESULT DreamBrowser::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	SENSE_CONTROLLER_EVENT_TYPE eventType = pEvent->type;

	// TODO: Replace with interaction engine based events

	if (pEvent->state.type == CONTROLLER_RIGHT) {
		
		if (eventType == SENSE_CONTROLLER_TRIGGER_UP) {
			// TODO: mouse down 
			if (m_fBrowserActive) {
				WebBrowserMouseEvent webBrowserEvent;

				webBrowserEvent.pt = m_lastWebBrowserPoint;
				webBrowserEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;

				CR(m_pWebBrowserController->SendMouseClick(webBrowserEvent, true, 1));
			}
		}
		else if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
			if (m_fBrowserActive) {
				WebBrowserMouseEvent webBrowserEvent;

				webBrowserEvent.pt = m_lastWebBrowserPoint;
				webBrowserEvent.mouseButton = WebBrowserMouseEvent::MOUSE_BUTTON::LEFT;

				CR(m_pWebBrowserController->SendMouseClick(webBrowserEvent, false, 1));
			}
		}
	}

Error:
	return r;
}
*/

std::shared_ptr<texture> DreamBrowser::GetScreenTexture() {
	return m_pBrowserTexture;
}

//TODO: currently unused?
RESULT DreamBrowser::SetScreenTexture(texture *pTexture) {
	m_aspectRatio = (float)pTexture->GetWidth() / (float)pTexture->GetHeight();
	SetParams(GetOrigin(), m_diagonalSize, m_aspectRatio, m_vNormal);

	return m_pBrowserQuad->SetDiffuseTexture(pTexture);
}

DreamBrowser* DreamBrowser::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamBrowser *pDreamApp = new DreamBrowser(pDreamOS, pContext);
	return pDreamApp;
}


RESULT DreamBrowser::SetScrollFactor(int scrollFactor) {
	m_scrollFactor = scrollFactor;
	return R_PASS;
}
