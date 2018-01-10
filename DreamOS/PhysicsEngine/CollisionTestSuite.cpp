#include "CollisionTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "Primitives/DimPlane.h"

CollisionTestSuite::CollisionTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

CollisionTestSuite::~CollisionTestSuite() {
	// empty
}

RESULT CollisionTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestOBBOBB());

	CR(AddTestQuadQuad());

	CR(AddTestSphereOBB());

	CR(AddTestSphereQuad());

	CR(AddTestSphereSphere());

	CR(AddTestPlaneQuad());

	CR(AddTestPlaneOBB());

	CR(AddTestPlanePlane());

	CR(AddTestPlaneRay());

	CR(AddTestPlaneSphere());

	CR(AddTestRayInComposite());

	CR(AddTestScaledCompositeRay());

	CR(AddTestRayModel());

Error:
	return r;
}

RESULT CollisionTestSuite::SetupSkyboxPipeline(std::string strRenderShaderName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	m_pSceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pSceneGraph, "Failed to allocate Debug Scene Graph");

	CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode(strRenderShaderName);
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Reference Geometry Shader Program
	ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
	CN(pReferenceGeometryProgram);
	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	// Debug Overlay
	ProgramNode* pDebugOverlay = pHAL->MakeProgramNode("debug_overlay");
	CN(pDebugOverlay);
	CR(pDebugOverlay->ConnectToInput("scenegraph", m_pSceneGraph->Output("objectstore")));
	CR(pDebugOverlay->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pDebugOverlay->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	// Skybox
	ProgramNode* pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
	CN(pSkyboxProgram);
	CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pDebugOverlay->Output("output_framebuffer")));

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

	light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

Error:
	return r;
}

RESULT CollisionTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests

	CR(m_pSceneGraph->RemoveAllObjects());

	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestPlaneOBB() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct TestContext {
		DimPlane *pPlane = nullptr;
		volume *pVolume = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		//pTestContext->pPlane = m_pDreamOS->AddPlane(point(1.0f, 0.5f, -1.0f), vector(1.0f, 1.0f, 0.0f).Normal());
		pTestContext->pPlane = m_pDreamOS->AddPlane();
		CN(pTestContext->pPlane);
		pTestContext->pPlane->SetMaterialColors(COLOR_BLUE);
		pTestContext->pPlane->SetPosition(0.0f, -0.001f, 0.0f);
		//pTestContext->pPlane->SetPosition(0.0f, -1.0f, 0.0f);

		pTestContext->pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pTestContext->pVolume);
		pTestContext->pVolume->SetMaterialColors(COLOR_GREEN);
		pTestContext->pVolume->SetPosition(0.0f, 1.0f, 0.0f);
		pTestContext->pVolume->RotateByDeg(15.0f, 35.0f, 45.0f);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pPlane);
		CN(pTestContext->pVolume);

		pTestContext->pVolume->translateY(-0.0001f);

		//for (int i = 0; i < 4; i++)
		//	pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pPlane->Intersect(pTestContext->pVolume)) {

			pTestContext->pVolume->SetMaterialColors(COLOR_RED);

			CollisionManifold manifold = pTestContext->pPlane->Collide(pTestContext->pVolume);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}

		}
		else {
			pTestContext->pVolume->SetMaterialColors(COLOR_GREEN);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Plane vs OBB Test");
	pNewTest->SetTestDescription("Test Plane vs OBB");
	pNewTest->SetTestDuration(sTestTime);
	//pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: 
RESULT CollisionTestSuite::AddTestPlaneQuad() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct TestContext {
		DimPlane *pPlane = nullptr;
		quad *pQuad = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		//pTestContext->pPlane = m_pDreamOS->AddPlane(point(1.0f, 0.5f, -1.0f), vector(1.0f, 1.0f, 0.0f).Normal());
		pTestContext->pPlane = m_pDreamOS->AddPlane();
		CN(pTestContext->pPlane);
		pTestContext->pPlane->SetMaterialColors(COLOR_BLUE);
		pTestContext->pPlane->SetPosition(0.0f, -0.001f, 0.0f);

		pTestContext->pQuad = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector::jVector(1.0f));
		CN(pTestContext->pQuad);
		pTestContext->pQuad->SetMaterialColors(COLOR_GREEN);
		pTestContext->pQuad->SetPosition(0.0f, 1.0f, 0.0f);
		pTestContext->pQuad->RotateByDeg(45.0f, 0.0f, 32.0f);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pPlane);
		CN(pTestContext->pQuad);

		pTestContext->pQuad->translateY(-0.0001f);

		//for (int i = 0; i < 4; i++)
		//	pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pPlane->Intersect(pTestContext->pQuad)) {

			pTestContext->pQuad->SetMaterialColors(COLOR_RED);

			CollisionManifold manifold = pTestContext->pPlane->Collide(pTestContext->pQuad);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}

		}
		else {
			pTestContext->pQuad->SetMaterialColors(COLOR_GREEN);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Plane vs Quad Test");
	pNewTest->SetTestDescription("Test Plane vs Quad");
	pNewTest->SetTestDuration(sTestTime);
	//pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestPlaneSphere() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct TestContext {
		sphere *pSphere = nullptr;
		DimPlane *pPlane = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		pTestContext->pPlane = m_pDreamOS->AddPlane(point(1.0f, 0.5f, -1.0f), vector(1.0f, 1.0f, 0.0f).Normal());
		CN(pTestContext->pPlane);
		pTestContext->pPlane->SetMaterialColors(COLOR_BLUE);
		pTestContext->pPlane->SetPosition(0.0f, -0.001f, 0.0f);

		pTestContext->pSphere = m_pDreamOS->AddSphere(0.25f, 20, 20);
		CN(pTestContext->pSphere);
		pTestContext->pSphere->SetMaterialColors(COLOR_GREEN);
		pTestContext->pSphere->SetPosition(0.0f, 1.0f, 0.0f);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pPlane);
		CN(pTestContext->pSphere);

		pTestContext->pSphere->translateY(-0.0001f);

		//for (int i = 0; i < 4; i++)
		//	pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pPlane->Intersect(pTestContext->pSphere)) {

			pTestContext->pSphere->SetMaterialColors(COLOR_RED);

			CollisionManifold manifold = pTestContext->pPlane->Collide(pTestContext->pSphere);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
			
		}
		else {
			pTestContext->pSphere->SetMaterialColors(COLOR_GREEN);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Sphere vs Plane Test");
	pNewTest->SetTestDescription("Test Sphere vs Plane");
	pNewTest->SetTestDuration(sTestTime);
	//pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestPlaneRay() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct TestContext {
		DimPlane *pPlane = nullptr;
		DimRay *pRay = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		pTestContext->pPlane = m_pDreamOS->AddPlane();
		CN(pTestContext->pPlane);
		pTestContext->pPlane->SetMaterialColors(COLOR_BLUE);
		pTestContext->pPlane->SetPosition(0.0f, -1.0f, 0.0f);

		pTestContext->pRay = m_pDreamOS->AddRay(point(), vector(1.0f, -1.0f, 0.0f).Normal());
		CN(pTestContext->pRay);
		pTestContext->pRay->SetPosition(-3.0f, 1.0f, 0.0f);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pPlane);
		CN(pTestContext->pRay);

		pTestContext->pRay->translateX(0.0001f);
		pTestContext->pRay->RotateZByDeg(0.01f);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pPlane->Intersect(pTestContext->pRay->GetRay(true))) {

			CollisionManifold manifold = pTestContext->pPlane->Collide(pTestContext->pRay->GetRay(true));

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}

		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Plane vs Ray Test");
	pNewTest->SetTestDescription("Plane vs Ray Test");
	pNewTest->SetTestDuration(sTestTime);
	//pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestOBBOBB() {
	RESULT r = R_PASS;

	double sTestTime = 500.0f;

	enum class TestOrientation {
		EDGE_EDGE,
		POINT_FACE,
		POINT_EDGE,
		FACE_FACE,
		EDGE_FACE
	} testOrientation;

	testOrientation = TestOrientation::EDGE_EDGE;
	//testOrientation = TestOrientation::POINT_FACE;
	//testOrientation = TestOrientation::EDGE_FACE;
	//testOrientation = TestOrientation::FACE_FACE;

	struct TestContext {
		volume *pOBBA = nullptr;
		volume *pOBBB = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
		DimRay *pCollidePointRay[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		pTestContext->pOBBB = m_pDreamOS->AddVolume(1.0f);
		CN(pTestContext->pOBBB);
		pTestContext->pOBBB->SetMaterialColors(COLOR_RED);

		pTestContext->pOBBA = m_pDreamOS->AddVolume(1.0f);
		CN(pTestContext->pOBBA);
		pTestContext->pOBBA->SetMaterialColors(COLOR_BLUE);


		switch (testOrientation) {
		case TestOrientation::EDGE_EDGE: {
			pTestContext->pOBBA->SetPosition(2.0f, -1.5f, 0.0f);
			pTestContext->pOBBA->RotateByDeg(0.0f, 0.0f, 45.0f);

			pTestContext->pOBBB->SetPosition(2.0f, -0.2f, 0.0f);

			pTestContext->pOBBB->RotateByDeg(45.0f, 0.0f, 0.0f);
		} break;

		case TestOrientation::FACE_FACE: {
			pTestContext->pOBBA->SetPosition(2.0f, -1.5f, 0.0f);
			pTestContext->pOBBA->RotateByDeg(0.0f, 0.0f, 45.0f);

			pTestContext->pOBBB->SetPosition(2.0f, -0.2f, 0.0f);

			pTestContext->pOBBB->RotateByDeg(45.0f, 0.0f, 0.0f);
			pTestContext->pOBBB->RotateByDeg(0.0f, 0.0f, 53.0f);
		} break;

		case TestOrientation::POINT_FACE: {
			pTestContext->pOBBA->SetPosition(0.0f, -1.5f, 0.0f);

			pTestContext->pOBBB->SetPosition(0.0f, -0.25f, 0.0f);
			pTestContext->pOBBB->RotateByDeg(45.0f, 0.0f, 45.0f);
		} break;

		case TestOrientation::EDGE_FACE: {
			pTestContext->pOBBA->SetPosition(0.0f, -1.5f, 0.0f);

			pTestContext->pOBBB->SetPosition(0.0f, -0.35f, 0.0f);
			pTestContext->pOBBB->RotateByDeg(0.0f, 0.0f, 45.0f);
			pTestContext->pOBBB->RotateByDeg(0.0f, 180.0f, 0.0f);
		} break;
		}

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);
			pTestContext->pCollidePoint[i]->SetVisible(false);

			pTestContext->pCollidePointRay[i] = m_pDreamOS->MakeRay(point(), vector::jVector(-1.0f), 1.0f);
			CN(pTestContext->pCollidePointRay[i]);
			m_pSceneGraph->PushObject(pTestContext->pCollidePointRay[i]);
			pTestContext->pCollidePointRay[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pOBBA);
		CN(pTestContext->pOBBB);

		//pTestContext->pOBBB->translateY(-0.0001f);
		pTestContext->pOBBB->RotateZByDeg(0.04f);
		//pTestContext->pOBBB->RotateYByDeg(0.04f);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pOBBA->Intersect(pTestContext->pOBBB)) {

			CollisionManifold manifold = pTestContext->pOBBA->Collide(pTestContext->pOBBB);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					for (int i = 0; i < manifold.NumContacts(); i++) {
						pTestContext->pCollidePoint[i]->SetVisible(true);
						pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());

						ray rPoint = ray(manifold.GetContactPoint(i).GetPoint(), manifold.GetContactPoint(i).GetNormal());
						pTestContext->pCollidePointRay[i]->SetVisible(true);
						pTestContext->pCollidePointRay[i]->UpdateFromRay(rPoint);
						pTestContext->pCollidePointRay[i]->SetRayVertices(1.0f);
						pTestContext->pCollidePointRay[i]->UpdateBuffers();
					}

					//pTestContext->pCollidePoint[0]->SetVisible(true);
					//pTestContext->pCollidePoint[0]->SetOrigin(manifold.GetContactPoint());
				}
			}

			pTestContext->pOBBA->SetMaterialColors(COLOR_GREEN);
		}
		else {
			pTestContext->pOBBA->SetMaterialColors(COLOR_BLUE);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Plane vs Plane Test");
	pNewTest->SetTestDescription("Plane vs Plane Test");
	pNewTest->SetTestDuration(sTestTime);
	//pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestQuadQuad() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct TestContext {
		quad *pQuadA = nullptr;
		quad *pQuadB = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		pTestContext->pQuadA = m_pDreamOS->AddQuad(0.5f, 0.5f);
		CN(pTestContext->pQuadA);
		pTestContext->pQuadA->SetMaterialColors(COLOR_BLUE);
		pTestContext->pQuadA->SetPosition(0.0f, -1.0f, 0.0f);

		pTestContext->pQuadB = m_pDreamOS->AddQuad(0.5f, 0.5f);
		CN(pTestContext->pQuadB);
		pTestContext->pQuadB->SetMaterialColors(COLOR_RED);
		pTestContext->pQuadB->SetPosition(0.0f, 0.0f, 0.0f);
		pTestContext->pQuadB->RotateByDeg(90.0f, 0.0f, 45.0f);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pQuadA);
		CN(pTestContext->pQuadB);

		pTestContext->pQuadB->translateY(-0.0001f);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pQuadA->Intersect(pTestContext->pQuadB)) {

			CollisionManifold manifold = pTestContext->pQuadA->Collide(pTestContext->pQuadB);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}

			pTestContext->pQuadA->SetMaterialColors(COLOR_GREEN);
		}
		else {
			pTestContext->pQuadA->SetMaterialColors(COLOR_BLUE);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Plane vs Plane Test");
	pNewTest->SetTestDescription("Plane vs Plane Test");
	pNewTest->SetTestDuration(sTestTime);
	//pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestPlanePlane() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct TestContext {
		DimPlane *pPlaneA = nullptr;
		DimPlane *pPlaneB = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		pTestContext->pPlaneA = m_pDreamOS->AddPlane();
		CN(pTestContext->pPlaneA);
		pTestContext->pPlaneA->SetMaterialColors(COLOR_BLUE);
		pTestContext->pPlaneA->SetPosition(0.0f, -2.0f, 0.0f);

		pTestContext->pPlaneB = m_pDreamOS->AddPlane(point(), vector::jVector(-1.0f));
		CN(pTestContext->pPlaneB);
		pTestContext->pPlaneB->SetMaterialColors(COLOR_RED);
		pTestContext->pPlaneB->SetPosition(0.0f, 2.0f, 0.0f);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pPlaneA);
		CN(pTestContext->pPlaneB);

		//pTestContext->pPlaneB->translateX(0.0001f);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pPlaneA->Intersect(pTestContext->pPlaneB)) {

			CollisionManifold manifold = pTestContext->pPlaneA->Collide(pTestContext->pPlaneB);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}

			pTestContext->pPlaneA->SetMaterialColors(COLOR_GREEN);
		}
		else {
			pTestContext->pPlaneA->SetMaterialColors(COLOR_BLUE);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Plane vs Plane Test");
	pNewTest->SetTestDescription("Plane vs Plane Test");
	pNewTest->SetTestDuration(sTestTime);
	//pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// Sphere
RESULT CollisionTestSuite::AddTestSphereSphere() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct TestContext {
		sphere *pSphereA = nullptr;
		sphere *pSphereB = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		pTestContext->pSphereA = m_pDreamOS->AddSphere(0.3f, 10, 10);
		CN(pTestContext->pSphereA);
		pTestContext->pSphereA->SetMaterialColors(COLOR_BLUE);
		pTestContext->pSphereA->SetPosition(0.0f, 0.3f, 0.0f);

		pTestContext->pSphereB = m_pDreamOS->AddSphere(0.3f, 10, 10);
		CN(pTestContext->pSphereB);
		pTestContext->pSphereB->SetMaterialColors(COLOR_RED);
		pTestContext->pSphereB->SetPosition(0.0f, 1.0f, 0.0f);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pSphereA);
		CN(pTestContext->pSphereB);

		//pTestContext->pSphereA->translateY(0.0005f);
		pTestContext->pSphereB->translateY(-0.0005f);

		//for (int i = 0; i < 4; i++)
		//	pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pSphereA->Intersect(pTestContext->pSphereB)) {

			//CollisionManifold manifold = pTestContext->pSphereA->Collide(pTestContext->pSphereB);
			CollisionManifold manifold = pTestContext->pSphereB->Collide(pTestContext->pSphereA);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}

			pTestContext->pSphereA->SetMaterialColors(COLOR_GREEN);

			pTestContext->pSphereA->SetVisible(false);
			pTestContext->pSphereB->SetVisible(false);
		}
		else {

			pTestContext->pSphereA->SetVisible(true);
			pTestContext->pSphereB->SetVisible(true);

			pTestContext->pSphereA->SetMaterialColors(COLOR_BLUE);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Sphere vs Sphere Test");
	pNewTest->SetTestDescription("Sphere vs Sphere Test");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestSphereQuad() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct TestContext {
		sphere *pSphere = nullptr;
		quad *pQuad = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		pTestContext->pSphere = m_pDreamOS->AddSphere(0.3f, 10, 10);
		CN(pTestContext->pSphere);
		pTestContext->pSphere->SetMaterialColors(COLOR_BLUE);
		pTestContext->pSphere->SetPosition(0.1f, 0.0f, 0.1f);

		pTestContext->pQuad = m_pDreamOS->AddQuad(1.5f, 1.5f, 1, 1, nullptr, vector::jVector(1.0f));
		CN(pTestContext->pQuad);
		pTestContext->pQuad->SetMaterialColors(COLOR_RED);
		pTestContext->pQuad->SetPosition(0.0f, -1.0f, 0.0f);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pSphere);
		CN(pTestContext->pQuad);

		pTestContext->pSphere->translateY(-0.00025f);

		//for (int i = 0; i < 4; i++)
		//	pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pSphere->Intersect(pTestContext->pQuad)) {

			//CollisionManifold manifold = pTestContext->pQuad->Collide(pTestContext->pSphere);
			CollisionManifold manifold = pTestContext->pSphere->Collide(pTestContext->pQuad);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}

			pTestContext->pSphere->SetMaterialColors(COLOR_GREEN);
		}
		else {
			pTestContext->pSphere->SetMaterialColors(COLOR_BLUE);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Sphere vs Quad Test");
	pNewTest->SetTestDescription("Sphere vs Quad Test");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestSphereOBB() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct TestContext {
		sphere *pSphere = nullptr;
		volume *pVolume = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects
		pTestContext->pSphere = m_pDreamOS->AddSphere(0.3f, 10, 10);
		CN(pTestContext->pSphere);
		pTestContext->pSphere->SetMaterialColors(COLOR_BLUE);
		pTestContext->pSphere->SetPosition(0.1f, 1.5f, -0.1f);

		pTestContext->pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pTestContext->pVolume);
		pTestContext->pVolume->SetMaterialColors(COLOR_RED);
		pTestContext->pVolume->SetPosition(0.0f, 0.6f, 0.0f);
		pTestContext->pVolume->RotateByDeg(-24.0f, 0.0f, 15.0f);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pSphere);
		CN(pTestContext->pVolume);

		pTestContext->pSphere->translateY(-0.0005f);

		//for (int i = 0; i < 4; i++)
		//	pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pSphere->Intersect(pTestContext->pVolume)) {

			//CollisionManifold manifold = pTestContext->pQuad->Collide(pTestContext->pSphere);
			CollisionManifold manifold = pTestContext->pSphere->Collide(pTestContext->pVolume);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}

			pTestContext->pSphere->SetMaterialColors(COLOR_GREEN);
		}
		else {
			pTestContext->pSphere->SetMaterialColors(COLOR_BLUE);
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Sphere vs OBB Test");
	pNewTest->SetTestDescription("Sphere vs OBB Test");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestRayInComposite() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;

	struct RayTestContext : public Subscriber<InteractionObjectEvent> {
		composite *pRayComposite = nullptr;
		composite *pComposite = nullptr;
		std::shared_ptr<model> pModel = nullptr;
		std::shared_ptr<DimRay> pRay = nullptr;
		std::shared_ptr<volume> pVolume = nullptr;
		std::shared_ptr<sphere> pSphere = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };

		virtual RESULT Notify(InteractionObjectEvent *mEvent) override {
			RESULT r = R_PASS;

			for (int i = 0; i < 4; i++)
				pCollidePoint[i]->SetVisible(false);

			if (mEvent->m_numContacts > 0) {
				for (int i = 0; i < mEvent->m_numContacts; i++) {
					pCollidePoint[i]->SetVisible(true);
					pCollidePoint[i]->SetOrigin(mEvent->m_ptContact[i]);
				}
			}

			/*
			// handle event
			switch (mEvent->m_eventType) {
			case InteractionEventType::ELEMENT_INTERSECT_BEGAN: {
				//
			} break;

			case InteractionEventType::ELEMENT_INTERSECT_MOVED: {
				// stub
			} break;

			case InteractionEventType::ELEMENT_INTERSECT_ENDED: {
				//
			} break;

			}
			*/

			CR(r);

		Error:
			return r;
		}
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		CN(pTestContext);

		// Ray to composite

		pTestContext->pComposite = m_pDreamOS->AddComposite();
		CN(pTestContext->pComposite);
		pTestContext->pComposite->InitializeOBB();

		pTestContext->pVolume = pTestContext->pComposite->AddVolume(0.5);
		CN(pTestContext->pVolume);
		pTestContext->pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
		pTestContext->pVolume->SetVertexColor(color(COLOR_BLUE));

		pTestContext->pSphere = pTestContext->pComposite->AddSphere(0.25f, 10, 10);
		CN(pTestContext->pSphere);
		pTestContext->pSphere->SetPosition(point(1.0f, 0.0f, 0.0f));
		pTestContext->pSphere->SetVertexColor(color(COLOR_BLUE));

		pTestContext->pQuad = pTestContext->pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad);
		pTestContext->pQuad->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pTestContext->pQuad->SetVertexColor(color(COLOR_BLUE));

		pTestContext->pModel = pTestContext->pComposite->AddModel(L"\\face4\\untitled.obj");
		pTestContext->pModel->SetVertexColor(color(COLOR_BLUE), true);
		pTestContext->pModel->SetScale(0.02f);
		pTestContext->pModel->SetPosition(point(2.0f, -0.0f, 0.0f));

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		pTestContext->pComposite->SetPosition(point(0.0f, -1.0f, 0.0f));
		pTestContext->pComposite->SetScale(0.5f);

		//m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pComposite);
		//
		//CR(m_pDreamOS->RegisterEventSubscriber(pTestContext->pComposite, ELEMENT_INTERSECT_BEGAN, pTestContext));
		//CR(m_pDreamOS->RegisterEventSubscriber(pTestContext->pComposite, ELEMENT_INTERSECT_MOVED, pTestContext));
		//CR(m_pDreamOS->RegisterEventSubscriber(pTestContext->pComposite, ELEMENT_INTERSECT_ENDED, pTestContext));

		// The Ray
		///*
		pTestContext->pRayComposite = m_pDreamOS->AddComposite();
		CN(pTestContext->pRayComposite);
		pTestContext->pRayComposite->InitializeOBB();

		pTestContext->pRay = pTestContext->pRayComposite->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, -1.0f, 0.0f).Normal());
		CN(pTestContext->pRay);
		pTestContext->pRay->SetPosition(point(-1.0f, 1.0f, 0.0f));

		//CR(m_pDreamOS->AddInteractionObject(pTestContext->pRay.get()));

		pTestContext->pRayComposite->SetPosition(-3.0f, 1.0f, 0.0f);
		pTestContext->pRayComposite->RotateZByDeg(45.0f);
		//*/

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		CN(pTestContext->pComposite);
		CN(pTestContext->pRayComposite);
		CN(pTestContext->pRay);
		CN(pTestContext->pVolume);
		CN(pTestContext->pSphere);
		CN(pTestContext->pQuad);

		///*
		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for composite collisions using the ray

		if (pTestContext->pComposite->Intersect(pTestContext->pRay->GetRay(true))) {
			CollisionManifold manifold = pTestContext->pComposite->Collide(pTestContext->pRay->GetRay(true));

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}
		//*/

		pTestContext->pRayComposite->translateX(0.001f);

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Composite Objects");
	pNewTest->SetTestDescription("Ray intersection of various objects in a composite and resolving those points");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(1);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestScaledCompositeRay() {
	RESULT r = R_PASS;

	double sTestTime = 55.0f;
	int nRepeats = 1;
	static int nRepeatCounter = 0;

	struct RayTestContext {
		composite *pComposite = nullptr;
		DimRay *pRay = nullptr;
		std::shared_ptr<volume> pVolume = nullptr;
		std::shared_ptr<sphere> pSphere = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		CN(pTestContext);

		// Ray to composite

		pTestContext->pComposite = m_pDreamOS->AddComposite();
		CN(pTestContext->pComposite);

		composite *pComposite = pTestContext->pComposite;
		CN(pComposite);

		// Test the various bounding types
		switch (nRepeatCounter) {
		case 0: pComposite->InitializeOBB(); break;
		case 1: pComposite->InitializeAABB(); break;
		case 2: pComposite->InitializeBoundingSphere(); break;
		}
		pComposite->SetMass(1.0f);

		pTestContext->pVolume = pComposite->AddVolume(0.5);
		CN(pTestContext->pVolume);
		pTestContext->pVolume->SetMass(1.0f);
		pTestContext->pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
		pTestContext->pVolume->SetVertexColor(color(COLOR_BLUE));

		pTestContext->pSphere = pComposite->AddSphere(0.25f, 10, 10);
		CN(pTestContext->pSphere);
		pTestContext->pSphere->SetMass(1.0f);
		pTestContext->pSphere->SetPosition(point(1.0f, 0.0f, 0.0f));
		pTestContext->pSphere->SetVertexColor(color(COLOR_BLUE));

		pTestContext->pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad);
		pTestContext->pQuad->SetMass(1.0f);
		pTestContext->pQuad->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pTestContext->pQuad->SetVertexColor(color(COLOR_BLUE));

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);

			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);

			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		pComposite->SetPosition(point(0.0f, -1.0f, 0.0f));
		pComposite->SetScale(0.5f);

		// Add physics composite
		CR(m_pDreamOS->AddPhysicsObject(pComposite));

		// The Ray
		///*
		pTestContext->pRay = m_pDreamOS->AddRay(point(-3.0f, 2.0f, 0.0f), vector(0.5f, -1.0f, 0.0f).Normal());
		CN(pTestContext->pRay);

		///*
		pTestContext->pRay->SetMass(1.0f);
		pTestContext->pRay->SetVelocity(vector(0.4f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pRay));
		//*/

		nRepeatCounter++;

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		CN(pTestContext->pComposite);
		CN(pTestContext->pRay);
		CN(pTestContext->pVolume);
		CN(pTestContext->pSphere);
		CN(pTestContext->pQuad);


		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for composite collisions using the ray

		//if (pTestContext->pRay->Intersect(pTestContext->pComposite)) {
		if (pTestContext->pComposite->Intersect(pTestContext->pRay->GetRay())) {
			//CollisionManifold manifold = pTestContext->pRay->Collide(pTestContext->pComposite);
			CollisionManifold manifold = pTestContext->pComposite->Collide(pTestContext->pRay->GetRay());

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Composite Objects");
	pNewTest->SetTestDescription("Ray intersection of various objects in a composite and resolving those points");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestRayModel() {
	RESULT r = R_PASS;

	double sTestTime = 250.0f;
	int nRepeats = 1;
	const int numQuads = 4;

	struct RayTestContext {
		composite *pComposite = nullptr;
		DimRay *pRay = nullptr;
		std::shared_ptr<model> pModel = nullptr;
		std::shared_ptr<volume> pVolume = nullptr;
		sphere *pCollidePoint[4] = { nullptr };
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupSkyboxPipeline("minimal"));

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		double yPos = -1.0f;
		double xPos = 2.0f;

		// Ray to quads 
		int quadCount = 0;

		pTestContext->pComposite = m_pDreamOS->AddComposite();
		CN(pTestContext->pComposite);
		CR(pTestContext->pComposite->InitializeOBB());

		{
			// Model
			pTestContext->pModel = pTestContext->pComposite->AddModel(L"\\face4\\untitled.obj");
			pTestContext->pModel->SetVertexColor(color(COLOR_BLUE), true);

			//pTestContext->pModel = pTestContext->pComposite->AddModel(L"\\cube.obj");
			//pTestContext->pModel->SetVertexColor(color(COLOR_BLUE), true);
			//pTestContext->pModel->SetPosition(point(-0.5f, -0.5f, -0.5f));

			//pTestContext->pVolume = pTestContext->pComposite->AddVolume(1.0f);
			//pTestContext->pVolume->SetVertexColor(color(COLOR_GREEN), true);

			//pTestContext->pComposite->SetPosition(point(-0.5f, -2.0f, -0.0f));
			//pTestContext->pComposite->SetScale(0.5f);

			pTestContext->pComposite->SetPosition(point(0.0f, -5.0f, 0.0f));
			pTestContext->pComposite->SetScale(0.1f);

			for (int i = 0; i < 4; i++) {
				pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);

				m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetMaterialDiffuseColor(color(COLOR_BLUE));
				pTestContext->pCollidePoint[i]->SetVisible(false);
			}

			pTestContext->pRay = m_pDreamOS->AddRay(point(-3.0f, 2.0f, 0.0f), vector(0.5f, -1.0f, 0.0f).Normal());
			CN(pTestContext->pRay);

			///*
			pTestContext->pRay->SetMass(1.0f);
			pTestContext->pRay->SetVelocity(vector(0.4f, 0.0f, 0.0f));
			CR(m_pDreamOS->AddPhysicsObject(pTestContext->pRay));
			//*/
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		ray rCast;

		CN(pTestContext->pRay);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		//rCast = pTestContext->pRay->GetRay();
		CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));

		// Check for quad collisions using the ray
		if (pTestContext->pComposite->Intersect(rCast)) {
			CollisionManifold manifold = pTestContext->pComposite->Collide(rCast);
			//CollisionManifold manifold = pTestContext->pModel->Collide(rCast);
			//CollisionManifold manifold = pTestContext->pModel->GetFirstChild<mesh>()->Collide(rCast);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Quads in Composite");
	pNewTest->SetTestDescription("Ray intersection of quads oriented in various fashion in a composite");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}
