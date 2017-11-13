#include "DreamPeerApp.h"
#include "DreamOS.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "Primitives/font.h"

#include "Cloud/HTTP/HTTPController.h"
#include "Cloud/Environment/PeerConnection.h"
#include "UI/UIView.h"

DreamPeerApp::DreamPeerApp(DreamOS *pDOS, void *pContext) :
	DreamApp<DreamPeerApp>(pDOS, pContext),
	m_pDOS(pDOS),
	m_state(DreamPeerApp::state::UNINITIALIZED)
	//m_pPeerConnection(pPeerConnection)
{
	// empty
}

//DreamPeerApp::DreamPeerApp(DreamOS *pDOS, PeerConnection *pPeerConnection, void *pContext) :
//	DreamApp<DreamPeerApp>(pDOS, pContext),
//	m_pDOS(pDOS),
//	m_state(DreamPeerApp::state::UNINITIALIZED),
//	m_pPeerConnection(pPeerConnection)
//{
//	// empty
//}

DreamPeerApp* DreamPeerApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamPeerApp *pDreamApp = new DreamPeerApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamPeerApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	// Subscribers (children)
	//for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
	//	CR(GetDOS()->RegisterEventSubscriber((InteractionEventType)(i), this));
	//}

	CR(SetState(state::INITIALIZED));

	SetAppName("DreamPeerApp");
	SetAppDescription("A Dream User App");

	// NOTE: User Model is assigned externally
	GetComposite()->InitializeOBB();
	GetDOS()->AddObjectToInteractionGraph(GetComposite());

	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_BEGAN, this));
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_MOVED, this));
	CR(GetDOS()->RegisterEventSubscriber(GetComposite(), ELEMENT_INTERSECT_ENDED, this));

Error:
	return r;
}

RESULT DreamPeerApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamPeerApp::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

	m_pUserModel = nullptr;

Error:
	return r;
}

RESULT DreamPeerApp::Update(void *pContext) {
	RESULT r = R_PASS;

	// If pending user mode - add to composite here
	if (m_fPendingAssignedUserMode) {
		CN(m_pUserModel);
		CR(GetComposite()->AddObject(m_pUserModel));
		m_fPendingAssignedUserMode = false;
	}

	//if (m_pSphere == nullptr) {
	//	m_pSphere = GetDOS()->AddSphere(0.025f, 10, 10);
	//	CN(m_pSphere);
	//}

	/*
	if (m_pOrientationRay == nullptr) {
		m_pOrientationRay = GetComposite()->AddRay(point(0.0f), vector::kVector(-1.0f), 1.0f);
		CN(m_pOrientationRay);
		m_pOrientationRay->SetVisible(false);

		// Adding the line below will register the peer look ray and will trigger an 
		// event when the peer is looking at something that consumes these events
		//CR(GetDOS()->AddInteractionObject(m_pOrientationRay.get()));
	}
	//*/

	if (m_pPhantomVolume == nullptr) {
		m_pPhantomVolume = GetComposite()->AddVolume(2.0f);
		CN(m_pPhantomVolume);
		m_pPhantomVolume->SetVisible(false);
	}

	///*
	if (m_pNameComposite == nullptr) {
		m_pNameComposite = GetComposite()->AddComposite();
		
		GetDOS()->AddObjectToUIGraph(m_pNameComposite.get());
	}

	if (m_pFont == nullptr) {
		m_pFont = GetDOS()->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
		CN(m_pFont);
		m_pFont->SetLineHeight(NAME_LINE_HEIGHT);
		
	}

	if (m_pNameBackground == nullptr) {
		CN(m_pNameComposite);

		m_pNameBackground = m_pNameComposite->AddQuad(0.9f, 0.2f);
		CN(m_pNameBackground);

		m_pNameBackground->SetPosition(point(0.0f, NAMETAG_HEIGHT, -0.01f));
		
		m_pNameBackground->SetVisible(false);
		
		m_pTextBoxTexture = GetComposite()->MakeTexture(L"user-nametag-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		m_pNameBackground->SetDiffuseTexture(m_pTextBoxTexture.get());
		m_pNameBackground->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));	
	}

	if (m_pTextUserName == nullptr && m_strScreenName != "") {
		m_pTextUserName = std::shared_ptr<text>(GetDOS()->MakeText(
			m_pFont,
			m_strScreenName,
			0.9 - NAMETAG_BORDER,
			0.2 - NAMETAG_BORDER,
			text::flags::TRAIL_ELLIPSIS | text::flags::FIT_TO_SIZE | text::flags::RENDER_QUAD));
		CN(m_pTextUserName);

		m_pTextUserName->SetVisible(false);

		m_pTextUserName->SetPosition(point(0.0f, NAMETAG_HEIGHT, 0.0f), text::VerticalAlignment::MIDDLE, text::HorizontalAlignment::CENTER);
		m_pTextUserName->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));
		CR(m_pNameComposite->AddObject(m_pTextUserName));	
	}
	
	if (m_pUserModel != nullptr) {
		m_pNameComposite->SetPosition(m_pUserModel->GetHead()->GetPosition() + point(0.0f, 0.5f, 0.0f));
	}
	
	if (m_fGazeInteraction) {
		std::chrono::steady_clock::duration tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
		float msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
		if (msTimeNow - m_msTimeGazeStart > m_msTimeUserNameDelay) {
			ShowUserNameField();
			m_fGazeInteraction = false;
		}
	}

	//*/

Error:
	return r;
}

RESULT DreamPeerApp::Notify(InteractionObjectEvent *mEvent) {
	RESULT r = R_PASS;

	if (mEvent->m_pInteractionObject != nullptr) {
		CBR((mEvent->m_pInteractionObject != m_pOrientationRay.get()), R_SKIPPED);
		CNR(m_pUserModel, R_SKIPPED);
	}

	//if (m_pSphere != nullptr) {
	//	m_pSphere->SetPosition(mEvent->m_ptContact[0]);
	//}

	// handle event
	switch (mEvent->m_eventType) {
		case InteractionEventType::ELEMENT_INTERSECT_BEGAN: {
			if (m_pUserModel->IsVisible()) {
				std::chrono::steady_clock::duration tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
				m_msTimeGazeStart = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
				m_fGazeInteraction = true;
			}
		} break;

		case InteractionEventType::ELEMENT_INTERSECT_MOVED: {
			// stub
		} break;

		case InteractionEventType::ELEMENT_INTERSECT_ENDED: {
			m_fGazeInteraction = false;
			HideUserNameField();
		} break;

		case InteractionEventType::ELEMENT_COLLIDE_BEGAN: {
			// stub
		} break;

		case InteractionEventType::ELEMENT_COLLIDE_TRIGGER: {
			// stub
		} break;

		case InteractionEventType::ELEMENT_COLLIDE_MOVED: {
			// stub
		} break;

		case InteractionEventType::ELEMENT_COLLIDE_ENDED: {
			// stub
		} break;
	}

Error:
	return r;
}

RESULT DreamPeerApp::HideUserNameField() {
	RESULT r = R_PASS;
	
	auto fnStartCallback = [&](void *pContext) {
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		
		return R_PASS;
	};
	/*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pTextUserName.get(),
		m_hiddenColor,
		0.5,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/
	///*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pTextUserName.get(),
		m_hiddenColor,
		0.3,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pNameBackground.get(),
		m_hiddenColor,
		0.3,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/
Error:
	return r;
}

RESULT DreamPeerApp::ShowUserNameField() {
	RESULT r = R_PASS;
	
	auto fnStartCallback = [&](void *pContext) {

		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {

		return R_PASS;
	};	

	m_pNameComposite->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), GetCameraLookXZ()));
	//* quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));
	/*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pNameComposite.get(),
		m_visibleColor,
		0.5,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/
	///*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pNameBackground.get(),
		m_backgroundColor,
		0.3,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pTextUserName.get(),
		m_visibleColor,
		0.3,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/
Error:
	return r;
}

RESULT DreamPeerApp::RegisterDreamPeerObserver(DreamPeerAppObserver* pDreamPeerObserver) {
	RESULT r = R_PASS;

	CNM((pDreamPeerObserver), "Observer cannot be nullptr");
	CBM((m_pDreamPeerObserver == nullptr), "Can't overwrite dream peer observer");
	m_pDreamPeerObserver = pDreamPeerObserver;

Error:
	return r;
}

DreamPeerApp::state DreamPeerApp::GetState() {
	return m_state;
}

RESULT DreamPeerApp::SetState(DreamPeerApp::state peerState) {
	RESULT r = R_PASS;

	// TODO: Check for incorrect state changes?

	if (peerState != m_state) {
		
		m_state = peerState;

		if (m_pDreamPeerObserver != nullptr) {
			CR(m_pDreamPeerObserver->OnDreamPeerStateChange(this));
		}
	}

Error:
	return r;
}

long DreamPeerApp::GetPeerUserID() {
	return m_peerUserID;
}

PeerConnection* DreamPeerApp::GetPeerConnection() {
	return m_pPeerConnection;
}

RESULT DreamPeerApp::SetPeerConnection(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	CN(pPeerConnection);

	m_pPeerConnection = pPeerConnection;
	m_peerUserID = m_pPeerConnection->GetPeerUserID();	
	
	auto pUserControllerProxy = (UserControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
	m_strScreenName = pUserControllerProxy->GetPeerScreenName(m_peerUserID);

Error:
	return r;
}

std::shared_ptr<user> DreamPeerApp::GetUserModel() {
	return m_pUserModel;
}

RESULT DreamPeerApp::AssignUserModel(user* pUserModel) {
	RESULT r = R_PASS;

	CN(pUserModel);
	m_pUserModel = std::shared_ptr<user>(pUserModel);
	m_fPendingAssignedUserMode = true;

Error:
	return r;
}

// TODO: We should create a proper object pool design
RESULT DreamPeerApp::ReleaseUserModel() {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel = nullptr;

Error:
	return r;
}

RESULT DreamPeerApp::SetVisible(bool fVisible) {
	RESULT r = R_PASS;
	std::shared_ptr<hand> pHand = nullptr;

	CN(m_pUserModel);
	CR(m_pUserModel->SetVisible(fVisible, false));
	if (m_pNameBackground != nullptr)
		CR(m_pNameBackground->SetVisible(fVisible));
	if (m_pTextUserName != nullptr)
		CR(m_pTextUserName->SetVisible(fVisible));

	pHand = m_pUserModel->GetHand(HAND_TYPE::HAND_LEFT);
	CN(pHand);
	pHand->SetVisible(fVisible);

	pHand = m_pUserModel->GetHand(HAND_TYPE::HAND_RIGHT);
	CN(pHand);
	pHand->SetVisible(fVisible);

Error:
	return r;
}

RESULT DreamPeerApp::SetPosition(const point& ptPosition) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	//m_pUserModel->GetHead()->SetPosition(ptPosition);
	GetComposite()->SetPosition(ptPosition);

Error:
	return r;
}

RESULT DreamPeerApp::SetOrientation(const quaternion& qOrientation) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel->GetHead()->SetOrientation(qOrientation);

Error:
	return r;
}

RESULT DreamPeerApp::RotateByDeg(float degX, float degY, float degZ) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	//m_pUserModel->RotateByDeg(degX, degY, degZ);
	m_pUserModel->GetHead()->RotateByDeg(degX, degY, degZ);
	//GetComposite()->RotateByDeg(degX, degY, degZ);

Error:
	return r;
}

RESULT DreamPeerApp::UpdateMouth(float mouthScale) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel->UpdateMouth(mouthScale);

Error:
	return r;
}

RESULT DreamPeerApp::UpdateHand(const hand::HandState& handState) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel->UpdateHand(handState);

Error:
	return r;
}

WebRTCPeerConnectionProxy* DreamPeerApp::GetWebRTCPeerConnectionProxy() {
	RESULT r = R_PASS;

	WebRTCPeerConnectionProxy *pWebRTCPeerConnectionProxy = nullptr;

	CN(m_pDOS);

	pWebRTCPeerConnectionProxy = m_pDOS->GetWebRTCPeerConnectionProxy(m_pPeerConnection);

	return pWebRTCPeerConnectionProxy;
Error:
	return nullptr;
}

bool DreamPeerApp::IsPeerReady() {
	return m_peerConnectionState.fReceivedAndRespondedToHandshake;
}

RESULT DreamPeerApp::OnDataChannel() {
	RESULT r = R_PASS;

	m_peerConnectionState.fDataChannel = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeerApp::OnAudioChannel() {
	RESULT r = R_PASS;

	m_peerConnectionState.fAudioChannel = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeerApp::UpdatePeerHandshakeState() {
	RESULT r = R_PASS;

	if (m_peerConnectionState.fReceivedHandshakeAck && 
		m_peerConnectionState.fSentHandshakeRequestACK &&
		m_peerConnectionState.fAudioChannel && 
		m_peerConnectionState.fDataChannel
		) 
	{
		m_peerConnectionState.fReceivedAndRespondedToHandshake = true;
		CR(SetState(state::ESTABLISHED));
	}
	else {
		if (m_peerConnectionState.fReceivedHandshakeAck ||
			m_peerConnectionState.fSentHandshakeRequestACK)
		{
			CR(SetState(state::PENDING));
		}

		m_peerConnectionState.fReceivedAndRespondedToHandshake = false;
	}

Error:
	return r;
}

RESULT DreamPeerApp::SentHandshakeRequest() {
	RESULT r = R_PASS;

	CB((m_peerConnectionState.fSentHandshakeRequest == false));
	m_peerConnectionState.fSentHandshakeRequest = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeerApp::ReceivedHandshakeACK() {
	RESULT r = R_PASS;

	CB((m_peerConnectionState.fSentHandshakeRequest == true));

	m_peerConnectionState.fSentHandshakeRequest = false;
	m_peerConnectionState.fReceivedHandshakeAck = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeerApp::ReceivedHandshakeRequest() {
	RESULT r = R_PASS;

	m_peerConnectionState.fReceivedHandshakeRequest = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeerApp::SentHandshakeACK() {
	RESULT r = R_PASS;

	m_peerConnectionState.fReceivedHandshakeRequest = false;
	m_peerConnectionState.fSentHandshakeRequestACK = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

