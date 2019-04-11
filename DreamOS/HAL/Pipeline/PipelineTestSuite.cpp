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
	CR(AddTestDynamicPipe());

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

	// Add the test
	auto pNewTest = AddTest(testDestriptor);
	CN(pNewTest);

Error:
	return r;
}

RESULT PipelineTestSuite::AddTestDynamicPipe() {
	RESULT r = R_PASS;

	TestObject::TestDescriptor testDestriptor;

	testDestriptor.strTestName = "dynamicpipe";
	testDestriptor.strTestDescription = "Testing the dynamically changing the pipeline at runtime";
	testDestriptor.sDuration = 1000.0f;
	testDestriptor.nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	struct TestContext {
		ProgramNode *pRenderProgramNodeA = nullptr;
		ProgramNode *pRenderProgramNodeB = nullptr;
		ProgramNode *pRenderScreenQuad = nullptr;
	};

	testDestriptor.pContext = (void*)(new TestContext());

	// Initialize Code 
	testDestriptor.fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;

		m_pDreamOS->SetGravityState(false);

		// Set up the pipeline
		HALImp *pHAL = m_pDreamOS->GetHALImp();
		Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

		SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
		CNM(pDestSinkNode, "Destination sink node isn't set");

		CR(pHAL->MakeCurrentContext());

		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Buffer for sink
		pTestContext->pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pTestContext->pRenderScreenQuad);

		CR(pDestSinkNode->ConnectToAllInputs(pTestContext->pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		{
			light *pLight;
			pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, 0.0f, -0.5f));

			auto pSphere = m_pDreamOS->AddSphere(1.0f);
			CN(pSphere);
		}

	Error:
		return r;
	};

	// Update Code 
	testDestriptor.fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		static std::chrono::system_clock::time_point s_lastTime = std::chrono::system_clock::now();
		static long counter = 2;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext); 

		{
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			auto sDifference = std::chrono::duration_cast<std::chrono::seconds>(timeNow - s_lastTime).count();

			if (sDifference > 1) {
				HALImp *pHAL = m_pDreamOS->GetHALImp();
				CN(pHAL);
				
				if (counter % 2 == 0) {
					if (pTestContext->pRenderProgramNodeA != nullptr) {
						CRM(pTestContext->pRenderProgramNodeA->Disconnect(), "Disconnect failed");
					}

					if (pTestContext->pRenderProgramNodeB == nullptr) {
						pTestContext->pRenderProgramNodeB = pHAL->MakeProgramNode("blinnphong");
						CN(pTestContext->pRenderProgramNodeB);
						CR(pTestContext->pRenderProgramNodeB->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
						CR(pTestContext->pRenderProgramNodeB->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
					}

					CR(pTestContext->pRenderScreenQuad->ConnectToInput("input_framebuffer", pTestContext->pRenderProgramNodeB->Output("output_framebuffer")));

					counter++;
				}
				else {
					if (pTestContext->pRenderProgramNodeB != nullptr) {
						CRM(pTestContext->pRenderProgramNodeB->Disconnect(), "Disconnect failed");
					}
				
					if (pTestContext->pRenderProgramNodeA == nullptr) {
						pTestContext->pRenderProgramNodeA = pHAL->MakeProgramNode("minimal");
						CN(pTestContext->pRenderProgramNodeA);
						CR(pTestContext->pRenderProgramNodeA->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
						CR(pTestContext->pRenderProgramNodeA->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
					}
				
					CR(pTestContext->pRenderScreenQuad->ConnectToInput("input_framebuffer", pTestContext->pRenderProgramNodeA->Output("output_framebuffer")));
					counter++;
				}

				s_lastTime = timeNow;
			}
		}

	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(testDestriptor);
	CN(pNewTest);

Error:
	return r;
}


