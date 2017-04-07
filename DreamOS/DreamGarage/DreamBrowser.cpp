#include "DreamBrowser.h"
#include "DreamOS.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"
#include "WebBrowser/WebBrowserController.h"

DreamBrowser::DreamBrowser(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamBrowser>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

// TODO: Only update the rect
RESULT DreamBrowser::OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;

	CN(m_pBrowserTexture);
	CR(m_pBrowserTexture->Update((unsigned char*)(pBuffer), width, height, texture::PixelFormat::BGRA));
	
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
		CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	}

	// Controller
	//RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_MENU_UP, this);
	GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_TRIGGER_UP, this);
	GetDOS()->RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_TRIGGER_DOWN, this);

	SetAppName("DreamContentView");
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
	
	// Flip UV vertically
	m_pBrowserQuad->TransformUV(
		{ { 0.0f, 0.0f } },
		{ { 1.0f, 0.0f,
			0.0f, -1.0f } }
	);

	m_pBrowserQuad->SetMaterialAmbient(0.8f);

	// Set up and map the texture
	m_pBrowserTexture = GetComposite()->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_COLOR, pxWidth, pxHeight, texture::PixelFormat::RGBA, 4, &vectorByteBuffer[0], pxWidth * pxHeight * 4);
	m_pBrowserQuad->SetMaterialTexture(DimObj::MaterialTexture::Ambient, m_pBrowserTexture.get());
	m_pBrowserQuad->SetMaterialTexture(DimObj::MaterialTexture::Diffuse, m_pBrowserTexture.get());

	// Set up mouse / hand cursor model
	///*
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

	GetDOS()->AddInteractionObject(m_pBrowserQuad.get());

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

	// First apply transforms to the ptIntersectionContact 
	point ptAdjustedContact = inverse(m_pBrowserQuad->GetModelMatrix()) * ptIntersectionContact;

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

	switch (pEvent->m_eventType) {
		case ELEMENT_INTERSECT_BEGAN: {
			m_pPointerCursor->SetVisible(true);

			WebBrowserMouseEvent webBrowserMouseEvent;

			webBrowserMouseEvent.pt = GetRelativeBrowserPointFromContact(pEvent->m_ptContact[0]);

			CR(m_pWebBrowserController->SendMouseMove(webBrowserMouseEvent, false));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
			m_fBrowserActive = true;

			fUpdateMouse = true;
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
			

			//CR(m_pWebBrowserController->SendMouseClick(webBrowserMouseEvent, fMouseUp, 1));

			m_lastWebBrowserPoint = webBrowserMouseEvent.pt;
		} break;
	}

	// First point of contact
	if (fUpdateMouse) {
		m_pPointerCursor->SetOrigin(pEvent->m_ptContact[0]);
	}

Error:
	return r;
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

// TODO: replace with core code
ray DreamBrowser::GetHandRay() {

	RESULT r = R_PASS;

	ray rCast;
	hand *pRightHand = GetDOS()->GetHand(hand::HAND_TYPE::HAND_RIGHT);

	CBR(m_pBrowserQuad->IsVisible(), R_SKIPPED);
	if(pRightHand != nullptr) {
		point ptHand = pRightHand->GetPosition();

		//GetLookVector
		quaternion qHand = pRightHand->GetHandState().qOrientation;
		qHand.Normalize();

		/*
		if (m_pTestRayController != nullptr) {
		m_pTestRayController->SetPosition(ptHand);
		m_pTestRayController->SetOrientation(qHand);
		}
		//*/

		//TODO: investigate how to properly get look vector for controllers
		//vector vHandLook = qHand.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();

		vector vHandLook = RotationMatrix(qHand) * vector(0.0f, 0.0f, -1.0f);
		vHandLook.Normalize();

		vector vCast = vector(-vHandLook.x(), -vHandLook.y(), vHandLook.z());
		/*
		if (m_pTestRayController != nullptr) {
		m_pTestRayLookV->SetPosition(ptHand);
		m_pTestRayLookV->SetOrientation(quaternion(vHandLook));
		}
		//*/
		// Accommodate for composite collision bug
		//ptHand = ptHand + point(-10.0f * vCast);
		//rCast = ray(ptHand, vCast);
		//rCast = m_pTestRayController->GetRayFromVerts();

		rCast = ray(ptHand, vHandLook);
	}
	/*
	else {
		CR(GetDOS()->GetMouseRay(rCast, 0.0f));
	}
	*/

	return rCast;

Error:
	return ray(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, 0.0f));
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
	if (r != R_SKIPPED) {
		m_pBrowserQuad->TransformUV(
		{ { 0.0f, 0.0f } },
		{ { 1.0f, 0.0f,
			0.0f, -1.0f } }
		);
	}

	CR(m_pBrowserQuad->SetDirty());

	CR(m_pBrowserQuad->InitializeBoundingQuad(point(0.0f, 0.0f, 0.0f), GetWidth(), GetHeight(), GetNormal()));

Error:
	return r;
}

RESULT DreamBrowser::SetVisible(bool fVisible) {
	return m_pBrowserQuad->SetVisible(fVisible);
}

RESULT DreamBrowser::SetURI(std::string strURI) {
	RESULT r = R_PASS;

	CN(m_pWebBrowserController);
	CR(m_pWebBrowserController->LoadURL(strURI));

Error:
	return r;
}

// TODO: Remove
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
		/*
		else if (eventType == SENSE_CONTROLLER_MENU_UP) {
			OVERLAY_DEBUG_SET("event", "menu up");
			CR(m_pDreamUIBar->HandleMenuUp());
		}
		*/
	}

Error:
	return r;
}


RESULT DreamBrowser::SetScreenTexture(texture *pTexture) {
	m_aspectRatio = (float)pTexture->GetWidth() / (float)pTexture->GetHeight();
	SetParams(GetOrigin(), m_diagonalSize, m_aspectRatio, m_vNormal);

	return m_pBrowserQuad->SetColorTexture(pTexture);
}

DreamBrowser* DreamBrowser::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamBrowser *pDreamApp = new DreamBrowser(pDreamOS, pContext);
	return pDreamApp;
}
