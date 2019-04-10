#include "PipelineTestSuite.h"

#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"


PipelineTestSuite::PipelineTestSuite(DreamOS *pDreamOS) :
	DreamTestSuite("pipeline", pDreamOS)
{
	// empty
}

RESULT PipelineTestSuite::AddTests() {
	RESULT r = R_PASS;

	// Add the tests

	CR(AddTestEmptyPipeline());

Error:
	return r;
}

RESULT PipelineTestSuite::SetupTestSuite() {
	RESULT r = R_PASS;

	// empty

Error:
	return r;
}

RESULT PipelineTestSuite::AddTestEmptyPipeline() {
	RESULT r = R_PASS;

	TestObject::TestDescriptor testDestriptor;

	testDestriptor.strTestName = "emptypipeline";
	testDestriptor.strTestDescription = "Testing the pipeline with limited shader stages and an empty scene graph";
	testDestriptor.sDuration = 20.0f;
	testDestriptor.nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	testDestriptor.fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode*pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode;
		pRenderProgramNode = pHAL->MakeProgramNode("minimal");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Connected in parallel (order matters)
		// NOTE: Right now this won't work with mixing for example
		CR(pDestSinkNode->ConnectToAllInputs(pRenderProgramNode->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		// Objects (should be empty for test to work correctly)

		{
			// Making a sphere seems to flip the right bits for some reason
			//auto pSphere = m_pDreamOS->MakeSphere(1.0f);
			//CN(pSphere);
		}

	Error:
		return r;
	};

	testDestriptor.pContext = m_pDreamOS;

	// Add the test
	auto pNewTest = AddTest(testDestriptor);
	CN(pNewTest);

Error:
	return r;
}


