#include "DreamTestAndroidClient.h"

DreamTestAndroidClient::DreamTestAndroidClient() :
	m_version(DREAM_OS_VERSION_MAJOR, DREAM_OS_VERSION_MINOR, DREAM_OS_VERSION_MINOR_MINOR)
{
	UNUSED RESULT r = R_PASS;

Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

DreamTestAndroidClient::~DreamTestAndroidClient() {
	// empty
}

RESULT DreamTestAndroidClient::ConfigureSandbox() {
	RESULT r = R_PASS;

	Sandbox::configuration sandboxconfig;

	sandboxconfig.fUseHMD = false;
	sandboxconfig.fUseLeap = false;
	sandboxconfig.fMouseLook = true;
	sandboxconfig.fUseGamepad = true;

	sandboxconfig.fInitCloud = false;		// TODO: This is currently breaking stuff
	//sandboxconfig.fInitCloud = true;		// TODO: This is currently breaking stuff

	//sandboxconfig.fInitSound = true;
	sandboxconfig.fInitSound = false;

	sandboxconfig.fInitUserApp = false;		// Turn on for testing User app related functionality 
	sandboxconfig.fInitNamedPipe = true;
	sandboxconfig.fInitKeyboard = false;

	sandboxconfig.fHMDMirror = true;
	sandboxconfig.fHideWindow = false;
	sandboxconfig.f3rdPersonCamera = false;

	sandboxconfig.hmdType = HMD_ANY_AVAILABLE;

	SetSandboxConfiguration(sandboxconfig);

Error:
	return r;
}

ESULT DreamTestAndroidClient::LoadScene() {
	RESULT r = R_PASS;

	//std::vector<std::string> testStringValues;

	// Set up the HAL Configuration 
	HALImp::HALConfiguration halconf;
	halconf.fRenderReferenceGeometry = false;
	halconf.fDrawWireframe = false;
	halconf.fRenderProfiler = false;
	SetHALConfiguration(halconf);

Error:
	return r;
}

RESULT DreamTestAndroidClient::Update(void) {
	RESULT r = R_PASS;

	// TODO: 

Error:
	return r;
}

version DreamTestAndroidClient::GetDreamVersion() {
	return version("0.0.0");		// This is... a placeholder?
}

RESULT DreamTestAndroidClient::MakePipeline(CameraNode* pCamera, OGLProgram*& pRenderNode, OGLProgram*& pEndNode, Sandbox::PipelineType pipelineType) {
	return R_NOT_IMPLEMENTED_WARNING;
}

// Cloud
RESULT DreamTestAndroidClient::OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage* pDreamMessage) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT DreamTestAndroidClient::OnNewDreamPeer(DreamPeerApp* pDreamPeer) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT DreamTestAndroidClient::OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT DreamTestAndroidClient::OnNewSocketConnection(int seatPosition) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT DreamTestAndroidClient::OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection,
		const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames)
{
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT DreamTestAndroidClient::OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	return R_NOT_IMPLEMENTED_WARNING;
}