#include "DreamTestSuite.h"

#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "DreamGarage\DreamGamepadCameraApp.h"

DreamTestSuite::DreamTestSuite(std::string strName, DreamOS *pParentDreamOS) :
	TestSuite(strName),
	m_pDreamOS(pParentDreamOS)
{
	// empty
}

RESULT DreamTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

	// Reset the pipeline
	HALImp *pHAL;
	pHAL = m_pDreamOS->GetHALImp();

	Pipeline* pPipeline;
	pPipeline = pHAL->GetRenderPipelineHandle();
	CR(pPipeline->Reset(false));

Error:
	return r;
}

RESULT DreamTestSuite::SetupPipeline(std::string strRenderProgramName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	{

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode(strRenderProgramName);
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("uistage");
		CN(pUIProgramNode);
		CR(pUIProgramNode->ConnectToInput("clippingscenegraph", m_pDreamOS->GetUIClippingSceneGraphNode()->Output("objectstore")));
		CR(pUIProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetUISceneGraphNode()->Output("objectstore")));
		CR(pUIProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		//TODO: Matrix node
		//	CR(pUIProgramNode->ConnectToInput("clipping_matrix", &m_pClippingView))

		// Connect output as pass-thru to internal blend program
		//CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		CR(pUIProgramNode->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

	}

Error:
	return r;
}