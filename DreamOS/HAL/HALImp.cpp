#include "HALImp.h"

HALImp::HALImp() :
	m_pCamera(nullptr),
	m_pHMD(nullptr)
{
	memset(&m_HALConfiguration, 0, sizeof(HALImp::HALConfiguration));
	
	//m_HALConfiguration.fRenderReferenceGeometry = true;
}

HALImp::~HALImp() {
	// empty stub
}

RESULT HALImp::SetHALConfiguration(HALConfiguration halconf) {
	m_HALConfiguration = halconf;
	return R_PASS;
}

const HALImp::HALConfiguration& HALImp::GetHALConfiguration() {
	return m_HALConfiguration;
}

RESULT HALImp::SetRenderReferenceGeometry(bool fRenderReferenceGeometry) {
	m_HALConfiguration.fRenderReferenceGeometry = fRenderReferenceGeometry;
	return R_PASS;
}

bool HALImp::IsRenderReferenceGeometry() {
	return (bool)(m_HALConfiguration.fRenderReferenceGeometry);
}

stereocamera* HALImp::GetCamera() {
	return m_pCamera;
}

RESULT HALImp::SetCamera(stereocamera* pCamera) {
	RESULT r = R_PASS;

	CN(pCamera);
	m_pCamera = pCamera;

Error:
	return r;
}

RESULT HALImp::SetCameraOrientation(quaternion qOrientation) {
	m_pCamera->SetOrientation(qOrientation);

	m_pCamera->RotateBy(m_pCamera->GetOffsetOrientation());
	
	return R_PASS;
}

RESULT HALImp::SetCameraPositionDeviation(vector vDeviation) {
	m_pCamera->SetCameraPositionDeviation(vDeviation);
	return R_PASS;
}

RESULT HALImp::SetHMD(HMD *pHMD) {
	RESULT r = R_PASS;

	m_pHMD = pHMD;
	m_pCamera->SetHMD(pHMD);

//Error:
	return r;
}

RESULT HALImp::InitializeRenderPipeline() {
	RESULT r = R_PASS;

	m_pRenderPipeline = std::make_unique<Pipeline>();
	CN(m_pRenderPipeline);

Error:
	return r;
}

RESULT HALImp::Render(ObjectStore* pSceneGraph, stereocamera* pCamera, EYE_TYPE eye) {
	RESULT r = R_PASS;

	// TODO: Replace this with source nodes
	ObjectStoreImp *pObjectStore = pSceneGraph->GetSceneGraphStore();
	VirtualObj *pVirtualObj = nullptr;

	static EYE_TYPE lastEye = EYE_INVALID;

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	CR(ClearHALBuffers());
	CR(ConfigureHAL());

	// Commit frame to HMD
	if (m_pHMD) {
		m_pHMD->UnsetRenderSurface(eye);
		m_pHMD->CommitSwapChain(eye);
	}

	SetViewTarget(eye, pCamera->GetViewWidth(), pCamera->GetViewHeight());

	// Pipeline stuff
	CN(m_pRenderPipeline);
	CR(m_pRenderPipeline->RunPipeline());

	CR(FlushHALBuffers())

Error:
	return r;
}

RESULT HALImp::SetDrawWireframe(bool fDrawWireframe) {
	m_HALConfiguration.fDrawWireframe = fDrawWireframe;
	return R_PASS;
}

bool HALImp::IsDrawWireframe() {
	return m_HALConfiguration.fDrawWireframe;
}

RESULT HALImp::SetRenderProfiler(bool fRenderProfiler) {
	m_HALConfiguration.fRenderProfiler = fRenderProfiler;
	return R_PASS;
}

bool HALImp::IsRenderProfiler() {
	return m_HALConfiguration.fRenderProfiler;
}