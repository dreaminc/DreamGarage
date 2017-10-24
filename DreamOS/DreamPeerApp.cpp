#include "DreamPeerApp.h"
#include "DreamOS.h"

#include "Cloud/HTTP/HTTPController.h"
#include "Cloud/Environment/PeerConnection.h"

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

	if (m_pOrientationRay == nullptr) {
		m_pOrientationRay = GetComposite()->AddRay(point(0.0f), vector::kVector(-1.0f), 1.0f);
		CN(m_pOrientationRay);
		m_pOrientationRay->SetVisible(false);

		// Adding the line below will register the peer look ray and will trigger an 
		// event when the peer is looking at something that consumes these events
		//CR(GetDOS()->AddInteractionObject(m_pOrientationRay.get()));
	}

	if (m_pPhantomVolume == nullptr) {
		m_pPhantomVolume = GetComposite()->AddVolume(2.0f);
		CN(m_pPhantomVolume);
		m_pPhantomVolume->SetVisible(false);
	}

	///*
	if (m_pFont == nullptr) {
		m_pFont = GetDOS()->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
		CN(m_pFont);
	}

	if (m_pTextUserName == nullptr) {
		m_pTextUserName = std::shared_ptr<text>(GetDOS()->MakeText(
			m_pFont,
			"u mad bro",
			1.0,
			0.25,
			text::flags::SCALE_TO_FIT | text::flags::RENDER_QUAD));
		CN(m_pTextUserName);

		CR(GetComposite()->AddObject(m_pTextUserName));

		m_pTextUserName->SetPosition(point(0.0f, 1.0f, 0.0f));
		m_pTextUserName->SetOrientationOffsetDeg(90.0f, 180.0f, 0.0f);
		m_pTextUserName->SetVisible(false);
	}
	//*/

Error:
	return r;
}

RESULT DreamPeerApp::Notify(InteractionObjectEvent *mEvent) {
	RESULT r = R_PASS;

	if (mEvent->m_pInteractionObject != nullptr) {
		CBR((mEvent->m_pInteractionObject != m_pOrientationRay.get()), R_SKIPPED);
	}

	//if (m_pSphere != nullptr) {
	//	m_pSphere->SetPosition(mEvent->m_ptContact[0]);
	//}

	// handle event
	switch (mEvent->m_eventType) {
		case InteractionEventType::ELEMENT_INTERSECT_BEGAN: {
			m_pTextUserName->SetVisible(true);
		} break;

		case InteractionEventType::ELEMENT_INTERSECT_MOVED: {
			// stub
		} break;

		case InteractionEventType::ELEMENT_INTERSECT_ENDED: {
			m_pTextUserName->SetVisible(false);
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

RESULT DreamPeerApp::GetPeerProfile(long peerUserID) {
	RESULT r = R_PASS;

	std::cout << "load peer profile..." << std::endl;
	{
		HTTPResponse httpResponse;
	
		auto pUserControllerProxy = (UserControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
		std::string strAuthorizationToken = "Authorization: Token " + pUserControllerProxy->GetUserToken();

		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		std::string strAPIURL = pCommandLineManager->GetParameterValue("api.ip");
		std::string	strURI = strAPIURL + "/user/" + std::to_string(peerUserID);

		HTTPController *pHTTPController = HTTPController::instance();

		auto headers = HTTPController::ContentAcceptJson();
		headers.push_back(strAuthorizationToken);

		CBM((pHTTPController->GET(strURI, headers, httpResponse)), "User LoadProfile failed to post request");

		DEBUG_LINEOUT("GET returned %s", httpResponse.PullResponse().c_str());

		std::string strHttpResponse(httpResponse.PullResponse());
		strHttpResponse = strHttpResponse.substr(0, strHttpResponse.find('\r'));
		nlohmann::json jsonResponse = nlohmann::json::parse(strHttpResponse);

		if (peerUserID = jsonResponse["/data/id"_json_pointer].get<long>()) {
			m_pTextUserName->SetText(jsonResponse["/data/public_name"_json_pointer].get<std::string>());
		}

		DEBUG_LINEOUT("User Profile Loaded");

	}

Error:
	return r;
}

PeerConnection* DreamPeerApp::GetPeerConnection() {
	return m_pPeerConnection;
}

RESULT DreamPeerApp::SetPeerConnection(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	CN(pPeerConnection);
	m_pPeerConnection = pPeerConnection;
	m_peerUserID = m_pPeerConnection->GetPeerUserID();
	GetPeerProfile(m_peerUserID);

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

RESULT DreamPeerApp::SetVisible(bool fVisibile) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	CR(m_pUserModel->SetVisible(fVisibile));

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
	m_pTextUserName->SetOrientation(qOrientation);

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

