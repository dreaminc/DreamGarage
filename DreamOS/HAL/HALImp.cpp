#include "HALImp.h"

#include "HAl/Pipeline/ProgramNode.h"

#include "HAL/FlatProgram.h"

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

RESULT HALImp::SetViewport(const viewport &newViewport) {
	m_viewport = newViewport;
	return R_PASS;
}

RESULT HALImp::SetViewport(int pxWidth, int pxHeight) {
	return m_viewport.ResizeViewport(pxWidth, pxHeight);
}

const viewport& HALImp::GetViewport() {
	return m_viewport;
}

// TODO: Remove this, this will eventually just be a node
RESULT HALImp::RenderToTexture(FlatContext* pContext, stereocamera* pCamera) {
	RESULT r = R_PASS;

	framebuffer *pFramebuffer = nullptr;
	FlatProgram* pFlatProgram = nullptr;

	if (m_pFlatProgram == nullptr) {
		m_pFlatProgram = MakeProgramNode("flat");
		CN(m_pFlatProgram);
	}

	pFlatProgram = dynamic_cast<FlatProgram*>(m_pFlatProgram);
	CN(pFlatProgram);

	pFramebuffer = pContext->GetFramebuffer();

	/*
	m_pFlatProgram->SetInput<stereocamera>("camera", pCamera);
	m_pFlatProgram->SetInput<FlatContext>("flatcontext", pContext);
	m_pFlatProgram->SetInput<framebuffer>("framebuffer", pFramebuffer);
	*/

	pFlatProgram->SetFlatContext(pContext);
	pFlatProgram->SetCamera(pCamera);
	pFlatProgram->SetFlatFramebuffer(pContext->GetFramebuffer());

	m_pFlatProgram->ProcessNode(0);
	
Error:
	return r;
}

RESULT HALImp::InitializeRenderPipeline() {
	RESULT r = R_PASS;

	m_pRenderPipeline = std::make_unique<Pipeline>();
	CN(m_pRenderPipeline);

Error:
	return r;
}

RESULT HALImp::Render() {
	RESULT r = R_PASS;

	// Pipeline stuff
	m_pRenderPipeline->RunPipeline();

	FlushHALBuffers();

//Error:
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