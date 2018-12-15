#include "EnvironmentController.h"
#include "Cloud/CloudController.h"

#include "CameraController.h"

#include "DreamLogger/DreamLogger.h"

#include "Cloud/User/User.h"

#include "Cloud/Websockets/Websocket.h"
#include "Sandbox/CommandLineManager.h"
#include "json.hpp"

#include "PeerConnection.h"

#include "Primitives/Types/UID.h"
#include "Primitives/Types/guid.h"

#include "Cloud/CloudMessage.h"

#include "Core/Utilities.h"

#include "EnvironmentAsset.h"

EnvironmentController::EnvironmentController(Controller* pParentController, long environmentID) :
	Controller(pParentController),
	m_fConnected(false),
	m_fPendingMessage(false),
	m_pEnvironmentWebsocket(nullptr),
	m_pendingMessageID(0),
	m_state(state::UNINITIALIZED)
{
	m_environment = Environment(environmentID);
}

EnvironmentController::~EnvironmentController() {
	if (m_pEnvironmentWebsocket != nullptr) {
		m_pEnvironmentWebsocket->Stop();
	}
}

RESULT EnvironmentController::Initialize() {
	RESULT r = R_PASS;
	
	// Peer Connection Controller
	m_pPeerConnectionController = std::unique_ptr<PeerConnectionController>(new PeerConnectionController(this));
	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->Initialize());
	CR(m_pPeerConnectionController->RegisterPeerConnectionControllerObserver(this));

	m_pCameraController = std::unique_ptr<CameraController>(new CameraController(this));
	CN(m_pCameraController);
	CR(m_pCameraController->Initialize());

	CR(m_pCameraController->RegisterMethod("open", std::bind(&EnvironmentController::OnOpenCamera, this, std::placeholders::_1)));
	CR(m_pCameraController->RegisterMethod("close", std::bind(&EnvironmentController::OnCloseCamera, this, std::placeholders::_1)));
	CR(m_pCameraController->RegisterMethod("send_placement", std::bind(&EnvironmentController::OnSendCameraPlacement, this, std::placeholders::_1)));
	CR(m_pCameraController->RegisterMethod("receive_placement", std::bind(&EnvironmentController::OnReceiveCameraPlacement, this, std::placeholders::_1)));
	CR(m_pCameraController->RegisterMethod("stop_sending_placement", std::bind(&EnvironmentController::OnStopSendingCameraPlacement, this, std::placeholders::_1)));
	CR(m_pCameraController->RegisterMethod("stop_receiving_placement", std::bind(&EnvironmentController::OnStopReceivingCameraPlacement, this, std::placeholders::_1)));

	// Menu Controller
	m_pMenuController = std::make_unique<MenuController>(this);
	//m_pMenuController = std::unique_ptr<MenuController>(new MenuController(this));
	CN(m_pMenuController);
	CR(m_pMenuController->Initialize());
	//CR(m_pMenuController->RegisterPeerConnectionControllerObserver(this));

	// Register Methods
	CR(RegisterMethod("open", std::bind(&EnvironmentController::OnOpenAsset, this, std::placeholders::_1)));
	CR(RegisterMethod("close", std::bind(&EnvironmentController::OnCloseAsset, this, std::placeholders::_1)));
	CR(RegisterMethod("share", std::bind(&EnvironmentController::OnSharedAsset, this, std::placeholders::_1)));
	CR(RegisterMethod("send", std::bind(&EnvironmentController::OnSharedAsset, this, std::placeholders::_1)));
	CR(RegisterMethod("receive", std::bind(&EnvironmentController::OnReceiveAsset, this, std::placeholders::_1)));
	CR(RegisterMethod("stop_sending", std::bind(&EnvironmentController::OnStopSending, this, std::placeholders::_1)));
	CR(RegisterMethod("stop_receiving", std::bind(&EnvironmentController::OnStopReceiving, this, std::placeholders::_1)));

	CR(RegisterMethod("get_form", std::bind(&EnvironmentController::OnGetForm, this, std::placeholders::_1)));
	//TODO: no method currently for a stop_sharing response, but could potentially be used for error handling

Error:
	return r;
}

RESULT EnvironmentController::RegisterEnvironmentControllerObserver(EnvironmentControllerObserver* pEnvironmentControllerObserver) {
	RESULT r = R_PASS;

	CNM((pEnvironmentControllerObserver), "Observer cannot be nullptr");
	CBM((m_pEnvironmentControllerObserver == nullptr), "Can't overwrite environment observer");
	m_pEnvironmentControllerObserver = pEnvironmentControllerObserver;

Error:
	return r;
}

std::string EnvironmentController::GetMethodURI(EnvironmentMethod userMethod) {
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	std::string strURI = "";
	std::string ip = pCommandLineManager->GetParameterValue("ws.ip");

	switch (userMethod) {
		case EnvironmentMethod::CONNECT_SOCKET: {
			strURI = ip + "/environment/";
		} break;
	}

	return strURI;
}

RESULT EnvironmentController::InitializeWebsocket(std::string& strURI) {
	RESULT r = R_PASS;

	//m_pEnvironmentWebsocket = std::unique_ptr<Websocket>(new Websocket(strURI));

	m_pEnvironmentWebsocket = std::unique_ptr<Websocket>(
		new Websocket(strURI, std::bind(&EnvironmentController::HandleWebsocketMessage, this, ::_1),
							  std::bind(&EnvironmentController::HandleWebsocketConnectionOpen, this),
							  std::bind(&EnvironmentController::HandleWebsocketConnectionClose, this),
							  std::bind(&EnvironmentController::HandleWebsocketConnectionFailed, this)));
	CN(m_pEnvironmentWebsocket);

	m_state = state::SOCKET_INITIALIZED;

Error:
	return r;
}

RESULT EnvironmentController::SendEnvironmentSocketMessage(std::shared_ptr<CloudMessage> pCloudRequest, state newState) {
	RESULT r = R_PASS;

	CR(SendEnvironmentSocketData(pCloudRequest->GetJSONDataString(), newState));

Error:
	return r;
}

RESULT EnvironmentController::SendEnvironmentSocketData(const std::string& strData, state newState) {
	RESULT r = R_PASS;

	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	m_fPendingMessage = true;
	m_state = newState;

	DEBUG_LINEOUT("SendEnvironmentSocketData JSON: %s", strData.c_str());

	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");

Error:
	return r;
}

// TODO: FIX THIS ITS REALLY BAD (for testing)
static User s_user;

RESULT EnvironmentController::ConnectToEnvironmentSocket(User user, long environmentID) {
	RESULT r = R_PASS;

	//m_environment = Environment(user.GetDefaultEnvironmentID());

	SetEnvironmentID(user.GetDefaultEnvironmentID());

	m_environment = Environment(user.GetDefaultEnvironmentID());

	std::string strURI = GetMethodURI(EnvironmentMethod::CONNECT_SOCKET);
	strURI += std::to_string(m_environment.GetEnvironmentID()); 
	strURI += "/";

	DOSLOG(INFO, "Connecting to environment socket URL: %s", strURI.c_str());
	CR(InitializeWebsocket(strURI));

	s_user = user;
	m_pEnvironmentWebsocket->SetToken(user.GetToken());

	m_state = state::SOCKET_CONNECTING;
	CRM(m_pEnvironmentWebsocket->Start(), "Failed to start environment websocket");

	m_fConnected = true;

	DOSLOG(INFO, "[EnvironmentController] user connected to socket:user=%v", user);

Error:
	return r;
}

RESULT EnvironmentController::DisconnectFromEnvironmentSocket() {
	RESULT r = R_PASS;

	// close all peers

	// Close all peer connections
	CR(m_pPeerConnectionController->CloseAllPeerConnections());
	
	DOSLOG(INFO, "Disconnecting from Environment Socket");

	CNR(m_pEnvironmentWebsocket, R_SKIPPED);

	CR(m_pEnvironmentWebsocket->Stop());
	m_pEnvironmentWebsocket = nullptr;

Error:
	return r;
}

// TODO: this might be dead code
RESULT EnvironmentController::CreateEnvironmentUser(User user) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;
	std::string strSDPOffer;
	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());
	guid guidMessage;

	CNM(pParentCloudController, "Parent CloudController not found or null");

	// Moved to send
	//CN(m_pEnvironmentWebsocket);
	//CBM((m_fConnected), "Environment socket not connected");
	//CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");
	
	//strSDPOffer = pParentCloudController->GetSDPOfferString();

	// Set up the JSON data

	// TODO: change pending ID to guid instead
	//m_pendingMessageID = guidMessage.GetGUIDString();

	jsonData["id"] = guidMessage.GetGUIDString();
	//jsonData["token"] = user.GetToken();
	jsonData["method"] = "environmentuser.create";
	jsonData["params"] = {
		//{"sdp_offer", "{'foo': 'bar2'}"},
		{ "sdp_offer", strSDPOffer },
		{"user", user.GetUserID() },
		{"environment", environmentID}
	};
	jsonData["version"] = user.GetVersion().GetString(false);

	strData = jsonData.dump();
	DEBUG_LINEOUT("Create Environment User JSON: %s", strData.c_str());

	/*
	m_fPendingMessage = true;
	m_state = state::CREATING_ENVIRONMENT_USER;
	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");
	*/

	CR(SendEnvironmentSocketData(strData, state::CREATING_ENVIRONMENT_USER));

Error:
	return r;
}

// TODO: Move to PeerConnection for PeerConnection related calls?
nlohmann::json EnvironmentController::CreateEnvironmentMessage(User user, PeerConnection *pPeerConnection, std::string strMethod) {
	nlohmann::json jsonData;

	// Set up the JSON data
	guid guidMessage = guid();
	std::string strGUID = guidMessage.GetGUIDString();

	jsonData["id"] = guidMessage.GetGUIDString();
	//jsonData["token"] = user.GetToken();
	//jsonData["type"] = "response";
	jsonData["type"] = "request";

	//jsonData["method"] = std::string("environmentuser") + strMethod;
	jsonData["method"] = strMethod;

	jsonData["payload"] = nlohmann::json::object();
	if (pPeerConnection != nullptr) {
		jsonData["payload"]["peer_connection"] = pPeerConnection->GetPeerConnectionJSON();
	}

	jsonData["version"] = user.GetVersion().GetString(false);

	return jsonData;
}

RESULT EnvironmentController::SetSDPOffer(User user, PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;
	
	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	jsonData = CreateEnvironmentMessage(user, pPeerConnection, "peer_connection.set_offer");

	strData = jsonData.dump();
	DEBUG_LINEOUT("Set SDP Offer JSON: %s", strData.c_str());

	/*
	m_fPendingMessage = true;
	m_state = state::SET_SDP_OFFER;
	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");
	*/

	CR(SendEnvironmentSocketData(strData, state::SET_SDP_OFFER));

	DOSLOG(INFO, "[EnvironmentController] offer was sent to cloud, msg=%v", strData);

Error:
	return r;
}

RESULT EnvironmentController::SetSDPAnswer(User user, PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;

	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	jsonData = CreateEnvironmentMessage(user, pPeerConnection, "peer_connection.set_answer");

	strData = jsonData.dump();
	DEBUG_LINEOUT("Set SDP Offer JSON: %s", strData.c_str());

	/*
	m_fPendingMessage = true;
	m_state = state::SET_SDP_OFFER;
	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");
	*/

	CR(SendEnvironmentSocketData(strData, state::SET_SDP_ANSWER));

Error:
	return r;
}

RESULT EnvironmentController::SetOfferCandidates(User user, PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;

	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	jsonData = CreateEnvironmentMessage(user, pPeerConnection, "peer_connection.set_offer_candidates");

	strData = jsonData.dump();
	DEBUG_LINEOUT("Set Offer Candidates JSON: %s", strData.c_str());

	/*
	m_fPendingMessage = true;
	m_state = state::SET_OFFER_CANDIDATES;
	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");
	*/

	CR(SendEnvironmentSocketData(strData, state::SET_OFFER_CANDIDATES));

Error:
	return r;
}

// TODO: Lots of duplicated code 
RESULT EnvironmentController::SetAnswerCandidates(User user, PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;

	long environmentID = user.GetDefaultEnvironmentID();
	CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());

	CNM(pParentCloudController, "Parent CloudController not found or null");
	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	jsonData = CreateEnvironmentMessage(user, pPeerConnection, "peer_connection.set_answer_candidates");

	strData = jsonData.dump();
	DEBUG_LINEOUT("Set Answer Candidates JSON: %s", strData.c_str());

	/*
	m_fPendingMessage = true;
	m_state = state::SET_ANSWER_CANDIDATES;
	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");
	*/

	CR(SendEnvironmentSocketData(strData, state::SET_ANSWER_CANDIDATES));

Error:
	return r;
}

// TODO: Right now this is no different than create user... 
RESULT EnvironmentController::UpdateEnvironmentUser() {
	RESULT r = R_PASS;

	CR(CreateEnvironmentUser(s_user));

Error:
	return r;
}

// TODO: This might be deprecated 
RESULT EnvironmentController::GetEnvironmentPeerList(User user) {
	RESULT r = R_PASS;

	nlohmann::json jsonData;
	std::string strData;
	long environmentID = user.GetDefaultEnvironmentID();

	CN(m_pEnvironmentWebsocket);
	CBM((m_fConnected), "Environment socket not connected");
	CBM(m_pEnvironmentWebsocket->IsRunning(), "Environment socket not running");

	// Set up the JSON data
	m_pendingMessageID = UID().GetID();
	jsonData["id"] = m_pendingMessageID;
	//jsonData["token"] = user.GetToken();
	jsonData["method"] = "environmentuser.list";
	jsonData["params"] = "";
	jsonData["version"] = user.GetVersion().GetString(false);

	strData = jsonData.dump();
	DEBUG_LINEOUT("Get Environment User List JSON: %s", strData.c_str());

	/*
	m_fPendingMessage = true;
	m_state = state::ENVIRONMENT_PEER_LIST_REQUESTED;
	CRM(m_pEnvironmentWebsocket->Send(strData), "Failed to send JSON data");
	*/

	CR(SendEnvironmentSocketData(strData, state::ENVIRONMENT_PEER_LIST_REQUESTED));

Error:
	return r;
}

CLOUD_CONTROLLER_TYPE EnvironmentController::GetControllerType() {
	return CLOUD_CONTROLLER_TYPE::ENVIRONMENT; 
}

RESULT EnvironmentController::RequestOpenAsset(std::string strStorageProviderScope, std::string strPath, std::string strTitle) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	std::string strData;
	guid guidMessage;
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["environment_asset"] = nlohmann::json::object();
	jsonPayload["environment_asset"]["path"] = strPath;

	//jsonPayload["environment_asset"]["storage_provider_scope"] = strStorageProviderScope;
	jsonPayload["environment_asset"]["scope"] = strStorageProviderScope;
	jsonPayload["environment_asset"]["title"] = strTitle;

	pCloudRequest = CloudMessage::CreateRequest(GetCloudController(), jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("environment_asset.open"));

	CR(SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::ENVIRONMENT_ASSET_OPEN));

Error:
	return r;
}

RESULT EnvironmentController::RequestOpenCamera() {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	std::string strData;
	guid guidMessage;
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["environment_camera"] = nlohmann::json::object();
	//jsonPayload["environment_camera"]["path"] = "";

	//jsonPayload["environment_asset"]["storage_provider_scope"] = strStorageProviderScope;
	jsonPayload["environment_camera"]["scope"] = "MenuProviderScope.CameraMenuProvider";
	jsonPayload["environment_camera"]["storage_provider_scope"] = "MenuProviderScope.CameraMenuProvider";
	jsonPayload["environment_camera"]["title"] = "Dream Virtual Camera";
	jsonPayload["environment_camera"]["user"] = GetCloudController()->GetUserID();

	pCloudRequest = CloudMessage::CreateRequest(GetCloudController(), jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("environment_camera.open"));

	CR(SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::ENVIRONMENT_CAMERA_OPEN));

Error:
	return r;
}

RESULT EnvironmentController::RequestCloseCamera(long assetID) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	std::string strData;
	guid guidMessage;
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["environment_camera"] = nlohmann::json::object();
	jsonPayload["environment_camera"]["id"] = assetID;

	pCloudRequest = CloudMessage::CreateRequest(GetCloudController(), jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("environment_camera.close"));

	CR(SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::ENVIRONMENT_CAMERA_CLOSE));

Error:
	return r;
}

RESULT EnvironmentController::RequestShareCamera(long assetID) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	std::string strData;
	guid guidMessage;
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["environment_camera"] = nlohmann::json::object();
	jsonPayload["environment_camera"]["id"] = assetID;

	pCloudRequest = CloudMessage::CreateRequest(GetCloudController(), jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("environment_camera.share_placement"));

	CR(SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::ENVIRONMENT_CAMERA_SHARE_PLACEMENT));

Error:
	return r;
}

RESULT EnvironmentController::RequestCloseAsset(long assetID) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	std::string strData;
	guid guidMessage;
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["environment_asset"] = nlohmann::json::object();
	jsonPayload["environment_asset"]["id"] = assetID;

	pCloudRequest = CloudMessage::CreateRequest(GetCloudController(), jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("environment_asset.close"));

	CR(SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::ENVIRONMENT_ASSET_CLOSE));

Error:
	return r;
}

RESULT EnvironmentController::RequestShareAsset(long assetID) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	std::string strData;
	guid guidMessage;
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["environment_asset"] = nlohmann::json::object();
	jsonPayload["environment_asset"]["id"] = assetID;

	pCloudRequest = CloudMessage::CreateRequest(GetCloudController(), jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("environment_asset.share"));

	CR(SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::ENVIRONMENT_ASSET_SHARE));

Error:
	return r;
}

RESULT EnvironmentController::RequestStopSharing(long assetID) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	std::string strData;
	guid guidMessage;
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["environment_asset"] = nlohmann::json::object();
	jsonPayload["environment_asset"]["id"] = assetID;

	pCloudRequest = CloudMessage::CreateRequest(GetCloudController(), jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("environment_asset.stop_sharing"));

	CR(SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::ENVIRONMENT_STOP_SHARING));

Error:
	return r;
}

RESULT EnvironmentController::RequestForm(std::string key) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload;
	CloudController *pParentCloudController = GetCloudController();
	std::shared_ptr<CloudMessage> pCloudRequest = nullptr;

	jsonPayload["form"] = nlohmann::json::object();
	jsonPayload["form"]["key"] = key;

	pCloudRequest = CloudMessage::CreateRequest(pParentCloudController, jsonPayload);
	CN(pCloudRequest);
	CR(pCloudRequest->SetControllerMethod("form.get_form"));

	auto pEnvironmentController = dynamic_cast<EnvironmentController*>(pParentCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentController);
	CR(pEnvironmentController->SendEnvironmentSocketMessage(pCloudRequest, EnvironmentController::state::FORM_GET_FORM));

Error:
	return r;

}

RESULT EnvironmentController::PrintEnvironmentPeerList() {
	DEBUG_LINEOUT("%d Peers Environment: %d", (int)(m_environmentPeers.size()), (int)(m_environment.GetEnvironmentID()));
	for (auto &peer : m_environmentPeers)
		peer.PrintEnvironmentPeer();

	return R_PASS;
}

bool EnvironmentController::HasPeerConnections() {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);

	return m_pPeerConnectionController->HasPeerConnections();

Error:
	return false;
}

long EnvironmentController::GetUserID() {
	if (m_pEnvironmentControllerObserver != nullptr) {
		return m_pEnvironmentControllerObserver->GetUserID();
	}

	return -1;
}

EnvironmentPeer *EnvironmentController::GetPeerByUserID(long userID) {
	for (auto &peer : m_environmentPeers)
		if (peer.GetUserID() == userID)
			return &(peer);

	return nullptr;
}

bool EnvironmentController::FindPeerByUserID(long userID) {
	for (auto &peer : m_environmentPeers) 
		if (peer.GetUserID() == userID)
			return true;
	
	return false;
}

RESULT EnvironmentController::ClearPeerList() {
	m_environmentPeers.clear();
	return R_PASS;
}

RESULT EnvironmentController::UpdatePeer(long userID, long environmentID, const std::string& strSDPOffer) {
	RESULT r = R_PASS;

	EnvironmentPeer *pPeer = GetPeerByUserID(userID);
	CNM(pPeer, "User ID %d not found in peer list", userID);

	CR(pPeer->UpdateSDPOffer(strSDPOffer));

	DEBUG_LINEOUT("Peer %d updated", userID);

Error:
	return r;
}

RESULT EnvironmentController::AddNewPeer(long userID, long environmentID, const std::string& strSDPOffer) {
	RESULT r = R_PASS;

	CBM((s_user.GetUserID() != userID), "User ID %d is self ID - not allowed", userID);
	
	{
		EnvironmentPeer environmentPeer(userID, environmentID, strSDPOffer);
		m_environmentPeers.push_back(environmentPeer);
	}

Error:
	return r;
}

bool EnvironmentController::IsUserIDConnected(long peerUserID) {
	return m_pPeerConnectionController->IsUserIDConnected(peerUserID);
}

/*
// TODO: This is temp
RESULT EnvironmentController::InitializeNewPeerConnection(bool fCreateOffer, bool fAddDataChannel) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->InitializeNewPeerConnection(fCreateOffer, fAddDataChannel));

Error:
	return r;
}
*/

RESULT EnvironmentController::OnSDPOfferSuccess(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	// TODO: Fix the s_user bullshit

	CBM((pPeerConnection->GetOfferUserID() == s_user.GetUserID()), "User ID mismatch offer user ID of peer connection");
	CR(SetSDPOffer(s_user, pPeerConnection));

	DOSLOG(INFO, "OnSDPOfferSuccess");

	// TOOD: based on pPeerConnection vs username answer or answer

Error:
	return r;
}

RESULT EnvironmentController::OnSDPAnswerSuccess(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	// TODO: Fix the s_user bullshit
	CBM((pPeerConnection->GetAnswerUserID() == s_user.GetUserID()), "User ID mismatch answer user ID of peer connection");
	CR(SetSDPAnswer(s_user, pPeerConnection));

	DOSLOG(INFO, "OnSDPAnswerSuccess");

	// TOOD: based on pPeerConnection vs username answer or answer

Error:
	return r;
}

RESULT EnvironmentController::OnICECandidatesGatheringDone(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	if (pPeerConnection->GetOfferUserID() == s_user.GetUserID()) {
		CR(SetOfferCandidates(s_user, pPeerConnection));
	}
	else if (pPeerConnection->GetAnswerUserID() == s_user.GetUserID()) {
		CR(SetAnswerCandidates(s_user, pPeerConnection));
	}


Error:
	return r;
}

RESULT EnvironmentController::OnOpenAsset(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_asset"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {
		std::shared_ptr<EnvironmentAsset> pEnvironmentAsset = std::make_shared<EnvironmentAsset>(jsonEnvironmentAsset);
		CN(pEnvironmentAsset);

		//CR(pEnvironmentAsset->PrintEnvironmentAsset());

		if (m_pEnvironmentControllerObserver != nullptr) {
			// Moving to Send/Receive paradigm
			CR(m_pEnvironmentControllerObserver->OnEnvironmentAsset(pEnvironmentAsset));
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnOpenCamera(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentCamera = jsonPayload["/environment_camera"_json_pointer];

	if (jsonEnvironmentCamera.size() != 0) {
		std::shared_ptr<EnvironmentAsset> pEnvironmentAsset = std::make_shared<EnvironmentAsset>(jsonEnvironmentCamera);
		CN(pEnvironmentAsset);

		pEnvironmentAsset->SetContentType("ContentControlType.Camera");

		if (m_pEnvironmentControllerObserver != nullptr) {

			// TODO: may need to be specific to camera
			
			CR(m_pEnvironmentControllerObserver->OnOpenCamera(pEnvironmentAsset));
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnCloseCamera(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_camera"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {

		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnCloseCamera());
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnSendCameraPlacement(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_camera"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {

		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnSendCameraPlacement());
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnReceiveCameraPlacement(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_camera"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {

		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnSendCameraPlacement());
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnStopSendingCameraPlacement(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_camera"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {

		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnSendCameraPlacement());
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnStopReceivingCameraPlacement(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_camera"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {

		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnSendCameraPlacement());
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnCloseAsset(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_asset"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {

		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnCloseAsset());
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnSharedAsset(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_asset"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {
		std::shared_ptr<EnvironmentAsset> pEnvironmentAsset = std::make_shared<EnvironmentAsset>(jsonEnvironmentAsset);
		CN(pEnvironmentAsset);

		//CR(pEnvironmentAsset->PrintEnvironmentAsset());

		if (m_pEnvironmentControllerObserver != nullptr) {
			// Moving to Send/Receive paradigm
			CR(m_pEnvironmentControllerObserver->OnShareAsset());
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnSendAsset(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_asset"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {
		std::shared_ptr<EnvironmentAsset> pEnvironmentAsset = std::make_shared<EnvironmentAsset>(jsonEnvironmentAsset);
		CN(pEnvironmentAsset);

		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnEnvironmentAsset(pEnvironmentAsset));
		}
	}

Error:
	return r;
}

RESULT EnvironmentController::OnReceiveAsset(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_asset"_json_pointer];
	//int peerID = jsonPayload["/user"_json_pointer].get<int>();

	//*
	if (jsonEnvironmentAsset.size() != 0) {
		std::shared_ptr<EnvironmentAsset> pEnvironmentAsset = std::make_shared<EnvironmentAsset>(jsonEnvironmentAsset);
		CN(pEnvironmentAsset);
		// actually doesn't need to do anything, OnVideoFrame in DOS does a peer connection check
		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnReceiveAsset(pEnvironmentAsset->GetUserID()));
		}
	}
	//*/

Error:
	return r;
}

RESULT EnvironmentController::OnStopReceiving(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_asset"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {
		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnStopReceiving());
		}
	}
Error:
	return r;
}

RESULT EnvironmentController::OnGetForm(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonForm = jsonPayload["/form"_json_pointer];

	// TODO: potentially build this out into a class similar to EnvironmentAsset
	CBR(!jsonForm.is_null(), R_SKIPPED);

	CBR(jsonForm["/key"_json_pointer].is_string(), R_SKIPPED);
	CBR(jsonForm["/title"_json_pointer].is_string(), R_SKIPPED);
	CBR(jsonForm["/url"_json_pointer].is_string(), R_SKIPPED);

	CR(m_pEnvironmentControllerObserver->OnGetForm(jsonForm["/key"_json_pointer].get<std::string>(),
												jsonForm["/title"_json_pointer].get<std::string>(),
												jsonForm["/url"_json_pointer].get<std::string>()));

Error:
	return r;
}

RESULT EnvironmentController::OnStopSending(std::shared_ptr<CloudMessage> pCloudMessage) {
	RESULT r = R_PASS;

	nlohmann::json jsonPayload = pCloudMessage->GetJSONPayload();
	nlohmann::json jsonEnvironmentAsset = jsonPayload["/environment_asset"_json_pointer];

	if (jsonEnvironmentAsset.size() != 0) {
		if (m_pEnvironmentControllerObserver != nullptr) {
			CR(m_pEnvironmentControllerObserver->OnStopSending());
		}
	}
Error:
	return r;
}

void EnvironmentController::HandleWebsocketMessage(const std::string& strMessage) {
	DEBUG_LINEOUT("HandleWebsocketMessage");

	nlohmann::json jsonCloudMessage = nlohmann::json::parse(strMessage);

	std::shared_ptr<CloudMessage> pCloudMessage = CloudMessage::Create(GetCloudController(), strMessage);

	if (jsonCloudMessage["/method"_json_pointer] == nullptr) {
		// message error

		DOSLOG(ERR, "[EnvironmentController] websocket msg error (could be a user already logged in)");

		return;
	}

	std::string strGUID = jsonCloudMessage["/id"_json_pointer].get<std::string>();
	std::string strType = jsonCloudMessage["/type"_json_pointer].get<std::string>();
	std::string strMethod = jsonCloudMessage["/method"_json_pointer].get<std::string>();
	
	std::vector<std::string> strTokens = util::TokenizeString(strMethod, '.');

	// Determine who to handle this
	// TODO: Move this over to CloudMessage instead

	if (strTokens[0] == "peer_connection") {
		nlohmann::json jsonPayload = jsonCloudMessage["/payload"_json_pointer];
		strMethod = strTokens[1];

		if (strType == "request") {
			DOSLOG(INFO, "[EnvironmentController] HandleSocketMessage REQUEST %v, %v", strMethod ,jsonPayload);
			
			m_pPeerConnectionController->HandleEnvironmentSocketRequest(strMethod, jsonPayload);
		}
		else if (strType == "response") {
			DOSLOG(INFO, "[EnvironmentController] HandleSocketMessage RESPONSE %v, %v", strMethod ,jsonPayload);
			
			m_pPeerConnectionController->HandleEnvironmentSocketResponse(strMethod, jsonPayload);
		}
		else {
			DOSLOG(ERR, "[EnvironmentController] websocket msg type unknown");
		}
	}
	else if (strTokens[0] == "socket_connection") {
		nlohmann::json jsonPayload = jsonCloudMessage["/payload"_json_pointer];
		strMethod = strTokens[1];
		
		if (strType == "response") {
			DOSLOG(INFO, "[EnvironmentController] HandleSocketMessage RESPONSE %v, %v", strMethod ,jsonPayload);
			
			m_pPeerConnectionController->HandleEnvironmentSocketResponse(strMethod, jsonPayload);
		}
		else if (strType == "request") {
			DOSLOG(INFO, "[EnvironmentController] HandleSocketMessage REQUEST %v, %v", strMethod, jsonPayload);

			if (strMethod == "disconnect") {
				// This is now done in DreamGarage since server will close the socket automatically 
				//RESULT r = DisconnectFromEnvironmentSocket();
				
				auto pUserController = dynamic_cast<UserController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
				pUserController->PendLogout();
				
			}
		}
		else {
			DOSLOG(ERR, "[EnvironmentController] websocket msg type unknown");
		}

	}
	else {
		DOSLOG(ERR, "[EnvironmentController] websocket msg method unknown");
	}

	if (pCloudMessage->GetController() == "menu") {
		m_pMenuController->HandleEnvironmentSocketMessage(pCloudMessage);
	}
	else if (pCloudMessage->GetController() == "environment_asset") {
		RESULT r = HandleOnMethodCallback(pCloudMessage);
		// TODO: Handle error 
	}
	else if (pCloudMessage->GetController() == "environment_camera") {
		m_pCameraController->HandleOnMethodCallback(pCloudMessage);
	}

	//TODO: split form into separate controller(?)
	else if (pCloudMessage->GetController() == "user") {
		auto pUserController = dynamic_cast<UserController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
		pUserController->HandleEnvironmentSocketMessage(pCloudMessage);
	}

	else if (pCloudMessage->GetController() == "form") {
		RESULT r = HandleOnMethodCallback(pCloudMessage);
	}


	/*
	DEBUG_LINEOUT("HandleWebsocketMessage id:%d statuscode: %d pending:%d pending id:%d state: 0x%x", 
		(int)(id), (int)(statusCode), m_fPendingMessage, (int)(m_pendingMessageID), m_state);

	if (m_fPendingMessage && (m_pendingMessageID == id)) {
		m_fPendingMessage = false;
		m_pendingMessageID = 0;

		switch (m_state) {
			case state::CREATING_ENVIRONMENT_USER: {
				// Environment User Created 
				DEBUG_LINEOUT("Environment User Created Successfully");
				m_state = state::ENVIRONMENT_USER_CREATED;
				GetEnvironmentPeerList(s_user); // TODO: s_user is dumb
			} break;

			case state::ENVIRONMENT_PEER_LIST_REQUESTED: {
				// Peer list received 
				DEBUG_LINEOUT("Environment Peer List Received");
				m_state = state::ENVIRONMENT_PEER_LIST_RECEIVED;
				ClearPeerList();

				// Save environment peer list
				nlohmann::json jsonPeerList = jsonCloudResponse["/data"_json_pointer];

				if (jsonPeerList.size() > 0) {
					for (nlohmann::json::iterator it = jsonPeerList.begin(); it != jsonPeerList.end(); ++it) {
						nlohmann::json jsonPeer = (*it);
						long userID = jsonPeer["/user"_json_pointer].get<long>();
						long environmentID = jsonPeer["/environment"_json_pointer].get<long>();
						std::string strSDPOffer = jsonPeer["/sdp_offer"_json_pointer].get<std::string>();

						AddNewPeer(userID, environmentID, strSDPOffer);
					}
				}

				PrintEnvironmentPeerList();

				DEBUG_LINEOUT("Environment connected and ready");
				m_state = state::ENVIRONMENT_CONNECTED_AND_READY;

			} break;

			
		}
	}
	else {
		CloudController *pParentCloudController = dynamic_cast<CloudController*>(GetParentController());
		if (pParentCloudController != nullptr) {
			switch (m_state) {
				case state::ENVIRONMENT_CONNECTED_AND_READY: {
					// TODO: Message type not implemented
					DEBUG_LINEOUT("New Environment Peer Connected");
					
					/*
					m_state = state::ENVIRONMENT_CONNECTED_AND_READY;
					GetEnvironmentPeerList(s_user); // TODO: s_user is dumb
					//*//*

					/*
					nlohmann::json jsonPeer = jsonCloudResponse["/data"_json_pointer];

					long userID = jsonPeer["/user"_json_pointer].get<long>();
					long environmentID = jsonPeer["/environment"_json_pointer].get<long>();

					std::string strSDPOffer = jsonPeer["/sdp_offer"_json_pointer].get<std::string>();

					if (FindPeerByUserID(userID)) {
						UpdatePeer(userID, environmentID, strSDPOffer);
					}
					else {
						AddNewPeer(userID, environmentID, strSDPOffer);
					}

					PrintEnvironmentPeerList();


					pParentCloudController->CreateSDPOfferAnswer(strSDPOffer);

					/*
					// Attempt to connect here
					std::string strType = jsonPeer["sdp_offer"]["type"].get<std::string>();
					if (strType == "offer") {
						pParentCloudController->CreateSDPOfferAnswer(strSDPOffer);
					}
					else if (strType == "answer") {
						// This is the update to the SDP info - so it's an answer and has the candidates
						// TODO: ICE Candidates / or change the name to handle SDP offer updates
						pParentCloudController->CreateSDPOfferAnswer(strSDPOffer);
					}
					//*//*
				} break;
			}
		}
		else {
			DEBUG_LINEOUT("Cannot handle message, cloud controller not found");
		}
	}
	*/
}

void EnvironmentController::HandleWebsocketConnectionOpen() {
	DEBUG_LINEOUT("HandleWebsocketConnectionOpen");

	m_state = state::SOCKET_CONNECTED;

	// Try to Create Environment User
	// Create Environment User
	// TODO: This is dumb
	//CreateEnvironmentUser(s_user);
}

void EnvironmentController::HandleWebsocketConnectionClose() {
	DEBUG_LINEOUT("HandleWebsocketConnectionClose");
}

void EnvironmentController::HandleWebsocketConnectionFailed() {
	DEBUG_LINEOUT("HandleWebsocketConnectionFailed");
}

RESULT EnvironmentController::SendDataChannelStringMessage(int peerID, std::string& strMessage) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->SendDataChannelStringMessage(peerID, strMessage));

Error:
	return r;
}

RESULT EnvironmentController::SendDataChannelMessage(int peerID, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->SendDataChannelMessage(peerID, pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

RESULT EnvironmentController::BroadcastDataChannelStringMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->BroadcastDataChannelStringMessage(strMessage));

Error:
	return r;
}

RESULT EnvironmentController::BroadcastDataChannelMessage(uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->BroadcastDataChannelMessage(pDataChannelBuffer, pDataChannelBuffer_n));

Error:
	return r;
}

// Video
RESULT EnvironmentController::BroadcastVideoFrame(uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->BroadcastVideoFrame(pVideoFrameBuffer, pxWidth, pxHeight, channels));

Error:
	return r;
}

RESULT EnvironmentController::StartVideoStreaming(int pxDesiredWidth, int pxDesiredHeight, int desiredFPS, PIXEL_FORMAT pixelFormat) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->StartVideoStreaming(pxDesiredWidth, pxDesiredHeight, desiredFPS, pixelFormat));

Error:
	return r;
}

RESULT EnvironmentController::StopVideoStreaming() {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CR(m_pPeerConnectionController->StopVideoStreaming());

Error:
	return r;
}

bool EnvironmentController::IsVideoStreamingRunning() {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	return m_pPeerConnectionController->IsVideoStreamingRunning();

Error:
	return false;
}

// Audio
RESULT EnvironmentController::BroadcastAudioPacket(const std::string &strAudioTrackLabel, const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	CN(m_pPeerConnectionController);
	CN(m_pPeerConnectionController->BroadcastAudioPacket(strAudioTrackLabel, pendingAudioPacket));

Error:
	return r;
}

float EnvironmentController::GetRunTimeMicAverage() {
	if (m_pPeerConnectionController != nullptr) {
		return m_pPeerConnectionController->GetRunTimeMicAverage();
	}

	return 0.0f;
}

RESULT EnvironmentController::SetUser(User currentUser) {
	return m_pPeerConnectionController->SetUser(currentUser);
}

RESULT EnvironmentController::SetTwilioNTSInformation(TwilioNTSInformation twilioNTSInformation) {
	return m_pPeerConnectionController->SetTwilioNTSInformation(twilioNTSInformation);
}

bool EnvironmentController::IsEnvironmentSocketConnected() {
	return m_fConnected;
}

MenuControllerProxy* EnvironmentController::GetMenuControllerProxy() {
	if(m_pMenuController != nullptr)
		return m_pMenuController->GetMenuControllerProxy();

	return nullptr;
}

WebRTCImpProxy* EnvironmentController::GetWebRTCControllerProxy() {
	if (m_pPeerConnectionController != nullptr)
		return m_pPeerConnectionController->GetWebRTCControllerProxy();

	return nullptr;
}

EnvironmentControllerProxy* EnvironmentController::GetEnvironmentControllerProxy() {
	return (EnvironmentControllerProxy*)(this);
}

RESULT EnvironmentController::OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnNewPeerConnection(userID, peerUserID, fOfferor, pPeerConnection));
	}

Error:
	return r;
}

RESULT EnvironmentController::OnNewSocketConnection(int seatPosition) {
	RESULT r = R_PASS;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnNewSocketConnection(seatPosition));
	}

Error:
	return r;
}

RESULT EnvironmentController::OnPeerConnectionClosed(PeerConnection *pPeerConnection) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnPeerConnectionClosed(pPeerConnection));
	}

Error:
	return r;
}

RESULT EnvironmentController::OnDataChannelStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnDataChannelStringMessage(pPeerConnection, strDataChannelMessage));
	}

Error:
	return r;
}

RESULT EnvironmentController::OnDataChannelMessage(PeerConnection* pPeerConnection, uint8_t *pDataChannelBuffer, int pDataChannelBuffer_n) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnDataChannelMessage(pPeerConnection, pDataChannelBuffer, pDataChannelBuffer_n));
	}

Error:
	return r;
}

RESULT EnvironmentController::OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioData, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnAudioData(strAudioTrackLabel, pPeerConnection, pAudioData, bitsPerSample, samplingRate, channels, frames));
	}

Error:
	return r;
}

RESULT EnvironmentController::OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnVideoFrame(pPeerConnection, pVideoFrameDataBuffer, pxWidth, pxHeight));
	}

Error:
	return r;
}

RESULT EnvironmentController::OnDataChannel(PeerConnection* pPeerConnection) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnDataChannel(pPeerConnection));
	}

Error:
	return r;
}

RESULT EnvironmentController::OnAudioChannel(PeerConnection* pPeerConnection) {
	RESULT r = R_NOT_IMPLEMENTED;

	if (m_pEnvironmentControllerObserver != nullptr) {
		CR(m_pEnvironmentControllerObserver->OnAudioChannel(pPeerConnection));
	}

Error:
	return r;
}
