#include "CollisionTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "Primitives/DimPlane.h"
#include "Primitives/HysteresisCylinder.h"

CollisionTestSuite::CollisionTestSuite(DreamOS *pDreamOS) :
	DreamTestSuite("collision"),
	m_pDreamOS(pDreamOS)
{
	// empty
}

CollisionTestSuite::~CollisionTestSuite() {
	// empty
}

RESULT CollisionTestSuite::AddTests() {
	RESULT r = R_PASS;

//	CR(AddTestRayInComposite());

	CR(AddTestHysteresisObj());

	CR(AddTestOBBOBB());

	CR(AddTestSphereSphere());

	CR(AddTestQuadQuad());

	CR(AddTestSphereOBB());

	CR(AddTestSphereQuad());

	CR(AddTestPlaneQuad());

	CR(AddTestPlaneOBB());

	CR(AddTestPlanePlane());

	CR(AddTestPlaneRay());

	CR(AddTestPlaneSphere());

	CR(AddTestScaledCompositeRay());

	CR(AddTestRayModel());

	CR(SetupPipeline("minimal"));

Error:
	return r;
}

RESULT CollisionTestSuite::SetupPipeline(std::string strRenderShaderName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	{

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

		// Skybox
		ProgramNode* pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// Debug Overlay
		ProgramNode* pDebugOverlay = pHAL->MakeProgramNode("debug_overlay");
		CN(pDebugOverlay);
		CR(pDebugOverlay->ConnectToInput("scenegraph", m_pSceneGraph->Output("objectstore")));
		CR(pDebugOverlay->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pDebugOverlay->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pDebugOverlay->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
	}

Error:
	return r;
}

RESULT CollisionTestSuite::SetupTestSuite() {
	RESULT r = R_PASS;

	CNM(m_pDreamOS, "DreamOS handle is not set");

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

		CR(SetupPipeline("minimal"));

		// Test Context
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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
	auto pNewTest = AddTest("planevsobb", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Test Plane vs OBB");
	pNewTest->SetTestDuration(sTestTime);

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

		CR(SetupPipeline("minimal"));

		// Test Context
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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
	auto pNewTest = AddTest("planevsquad", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Test Plane vs Quad");
	pNewTest->SetTestDuration(sTestTime);

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

		CR(SetupPipeline("minimal"));

		// Test Context
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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
	auto pNewTest = AddTest("spherevsplane", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Test Sphere vs Plane");
	pNewTest->SetTestDuration(sTestTime);

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

		CR(SetupPipeline("minimal"));

		// Test Context
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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
	auto pNewTest = AddTest("planevsray", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Plane vs Ray Test");
	pNewTest->SetTestDuration(sTestTime);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestOBBOBB() {
	RESULT r = R_PASS;

	double sTestTime = 500.0f;
	

	enum class TestOrientation {
		EDGE_EDGE,
		EDGE_EDGE_AABB,
		POINT_FACE,
		POINT_FACE_X,
		POINT_EDGE,
		FACE_FACE,
		AABB_AABB_X,
		AABB_AABB_Y,
		AABB_AABB_Z,
		EDGE_FACE
	} testOrientation;

	//int nRepeats = (int)(TestOrientation::EDGE_FACE);
	int nRepeats = 1;

	testOrientation = TestOrientation::EDGE_EDGE;
	//testOrientation = TestOrientation::EDGE_EDGE_AABB;
	//testOrientation = TestOrientation::POINT_FACE;
	//testOrientation = TestOrientation::POINT_FACE_X;
	//testOrientation = TestOrientation::EDGE_FACE;
	//testOrientation = TestOrientation::FACE_FACE;
	//testOrientation = TestOrientation::AABB_AABB_X;
	//testOrientation = TestOrientation::AABB_AABB_Y;
	//testOrientation = TestOrientation::AABB_AABB_Z;
	//testOrientation = (TestOrientation)(nRepeats);

	struct TestContext {
		volume *pOBBA = nullptr;
		volume *pOBBB = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
		DimRay *pCollidePointRay[4] = { nullptr, nullptr, nullptr, nullptr };
		float translateYA = 0.0f;
		float translateYB = 0.0f;
	} *pTestContext = new TestContext();

	

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		// Objects

		switch (testOrientation) {
			case TestOrientation::EDGE_EDGE: {
				pTestContext->pOBBB = m_pDreamOS->AddVolume(1.0f);
				pTestContext->pOBBB->SetPosition(2.0f, -1.5f, 0.0f);
				pTestContext->pOBBB->RotateByDeg(0.0f, 0.0f, 45.0f);

				pTestContext->pOBBA = m_pDreamOS->AddVolume(1.0f);
				pTestContext->pOBBA->SetPosition(2.0f, -0.2f, 0.0f);
				pTestContext->pOBBA->RotateByDeg(45.0f, 0.0f, 0.0f);

				auto pVol = m_pDreamOS->AddVolume(1.0f);
				pVol->SetMaterialColors(COLOR_AQUA);
			} break;

			case TestOrientation::EDGE_EDGE_AABB: {
				pTestContext->pOBBA = m_pDreamOS->AddVolume(1.0f);
				pTestContext->pOBBA->SetPosition(-1.25f, -0.25f, -0.5f);
				pTestContext->pOBBA->RotateByDeg(0.0f, 0.0f, 45.0f);

				pTestContext->pOBBB = m_pDreamOS->AddVolume(1.0f);
				pTestContext->pOBBB->SetPosition(-2.0f, -1.0f, 0.0f);
				//pTestContext->pOBBA->RotateByDeg(90.0f, 90.0f, 180.0f);
				//pTestContext->pOBBA->RotateByDeg(45.0f, 0.0f, 0.0f);

				auto pVol = m_pDreamOS->AddVolume(1.0f);
				pVol->SetMaterialColors(COLOR_AQUA);
			} break;

			case TestOrientation::FACE_FACE: {
				pTestContext->pOBBA = m_pDreamOS->AddVolume(1.0f);
				CN(pTestContext->pOBBA);
				pTestContext->pOBBA->SetPosition(-2.0f, -1.5f, 0.0f);
				//pTestContext->pOBBA->RotateByDeg(0.0f, 0.0f, 45.0f);

				pTestContext->pOBBB = m_pDreamOS->AddVolume(1.0f);
				CN(pTestContext->pOBBB);
				pTestContext->pOBBB->SetPosition(-1.1f, -0.6f, 0.0f);
				pTestContext->pOBBB->RotateByDeg(45.0f, 0.0f, 0.0f);
				//pTestContext->pOBBB->RotateByDeg(0.0f, 0.0f, 53.0f);
			} break;

			case TestOrientation::POINT_FACE: {
				pTestContext->pOBBA = m_pDreamOS->AddVolume(1.0f);
				pTestContext->pOBBA->SetPosition(0.0f, -1.5f, 0.0f);
				//pTestContext->pOBBA->RotateByDeg(0.0f, 0.0f, 90.0f);
				//pTestContext->pOBBA->RotateByDeg(0.0f, 0.0f, 180.0f);

				pTestContext->pOBBB = m_pDreamOS->AddVolume(1.0f);
				pTestContext->pOBBB->SetPosition(point(0.0f, -0.25f, 0.0f));
				pTestContext->pOBBB->RotateByDeg(45.0f, 0.0f, 45.0f);
				//pTestContext->pOBBB->RotateByDeg(45.0f, 0.0f, 0.0f);
				//pTestContext->pOBBB->RotateByDeg(0.0f, 0.0f, -35.0f);
			} break;

			case TestOrientation::POINT_FACE_X: {
				pTestContext->pOBBA = m_pDreamOS->AddVolume(1.0f);
				pTestContext->pOBBA->SetPosition(point(0.25f, -1.0f, 0.0f));
				pTestContext->pOBBA->RotateYByDeg(45.0f);
				pTestContext->pOBBA->RotateZByDeg(45.0f);

				pTestContext->pOBBB = m_pDreamOS->AddVolume(1.0f);
				pTestContext->pOBBB->SetPosition(point(-1.0f, -1.0f, 0.0f));

			} break;

			case TestOrientation::EDGE_FACE: {
				pTestContext->pOBBA = m_pDreamOS->AddVolume(0.5f, 0.5f, 1.0f);
				pTestContext->pOBBA->SetPosition(-1.0f, -0.45f, 0.0f);
				//pTestContext->pOBBA->RotateByDeg(0.0f, 0.0f, 135.0f);
				pTestContext->pOBBA->RotateByDeg(0.0f, 0.0f, 45.0f);
				//pTestContext->translateYB = -0.001f;

				pTestContext->pOBBB = m_pDreamOS->AddVolume(5.0f, 5.0f, 0.5f);
				pTestContext->pOBBB->SetPosition(0.0f, -1.0f, 0.0f);
				//pTestContext->pOBBB->RotateByDeg(0.0f, 0.0f, 45.0f);
				//pTestContext->pOBBB->RotateByDeg(0.0f, 180.0f, 0.0f);
				//pTestContext->translateYB = -0.001f;

			} break;

			case TestOrientation::AABB_AABB_X: {
				pTestContext->pOBBA = m_pDreamOS->AddVolume(1.0f);
				CN(pTestContext->pOBBA);
				pTestContext->pOBBA->SetPosition(-1.5f, 0.0f, 0.0f);

				pTestContext->pOBBB = m_pDreamOS->AddVolume(1.0f);
				CN(pTestContext->pOBBB);
				pTestContext->pOBBB->SetPosition(0.0f, -0.0f, 0.0f);
			} break;

			case TestOrientation::AABB_AABB_Y: {
				pTestContext->pOBBA = m_pDreamOS->AddVolume(1.0f);
				CN(pTestContext->pOBBA);
				pTestContext->pOBBA->SetPosition(0.0f, -1.5f, 0.0f);

				pTestContext->pOBBB = m_pDreamOS->AddVolume(1.0f);
				CN(pTestContext->pOBBB);
				pTestContext->pOBBB->SetPosition(0.0f, -0.0f, 0.0f);
			} break;

			case TestOrientation::AABB_AABB_Z: {
				pTestContext->pOBBA = m_pDreamOS->AddVolume(1.0f);
				CN(pTestContext->pOBBA);
				pTestContext->pOBBA->SetPosition(0.0f, -2.0f, 1.5f);

				pTestContext->pOBBB = m_pDreamOS->AddVolume(1.0f);
				CN(pTestContext->pOBBB);
				pTestContext->pOBBB->SetPosition(0.0f, -2.0f, 0.0f);
			} break;
		}

		pTestContext->pOBBA->SetMaterialColors(COLOR_BLUE);
		pTestContext->pOBBB->SetMaterialColors(COLOR_RED);

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

		//pTestContext->pOBBA->translateX(-0.0002f);
		//pTestContext->pOBBA->translateY(-0.0002f);

		//pTestContext->pOBBB->translateX(-0.0001f);
		//pTestContext->pOBBB->translateY(-0.00005f);
		//pTestContext->pOBBB->translateZ(0.0001f);

		pTestContext->pOBBA->RotateZByDeg(0.01f);
		//pTestContext->pOBBB->RotateZByDeg(0.02f);
		//pTestContext->pOBBB->RotateYByDeg(0.024f);
		//pTestContext->pOBBA->RotateZByDeg(-0.02f);
		//pTestContext->pOBBA->RotateXByDeg(-0.01f);
		//pTestContext->pOBBB->RotateYByDeg(0.04f);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for collisions 
		if (pTestContext->pOBBA->Intersect(pTestContext->pOBBB)) 
		{
			CollisionManifold manifold = pTestContext->pOBBA->Collide(pTestContext->pOBBB);
			//CollisionManifold manifold = pTestContext->pOBBB->Collide(pTestContext->pOBBA);
			
			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					for (int i = 0; i < manifold.NumContacts(); i++) {
						pTestContext->pCollidePoint[i]->SetVisible(true);
						pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
			
						ray rPoint = ray(manifold.GetContactPoint(i).GetPoint(), manifold.GetContactPoint(i).GetNormal());

						pTestContext->pCollidePointRay[i]->SetVisible(true);
						pTestContext->pCollidePointRay[i]->UpdateFromRay(rPoint);
						//pTestContext->pCollidePointRay[i]->SetRayVertices(manifold.GetContactPoint(i).GetPenetration() * 10.0f);
						
						pTestContext->pCollidePointRay[i]->SetRayVertices(1.0f);

						//if(manifold.GetContactPoint(i).GetPenetration() < 0.0f)
						//	pTestContext->pCollidePointRay[i]->SetRayVertices(-1.0f);
						//else
						//	pTestContext->pCollidePointRay[i]->SetRayVertices(1.0f);

						pTestContext->pCollidePointRay[i]->UpdateBuffers();
					}
			
					//pTestContext->pCollidePoint[0]->SetVisible(true);
					//pTestContext->pCollidePoint[0]->SetOrigin(manifold.GetContactPoint());
				}
			}

			pTestContext->pOBBA->SetMaterialColors(COLOR_GREEN);
		}
		else {
			//pTestContext->pOBBA->translateY(pTestContext->translateYA);
			//pTestContext->pOBBB->translateY(pTestContext->translateYB);
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
	auto pNewTest = AddTest("planevsplane", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Plane vs Plane Test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

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

		CR(SetupPipeline("minimal"));

		// Test Context
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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
	auto pNewTest = AddTest("planevsplane", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Plane vs Plane Test");
	pNewTest->SetTestDuration(sTestTime);

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

		CR(SetupPipeline("minimal"));

		// Test Context
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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
	auto pNewTest = AddTest("planevsplane", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Plane vs Plane Test");
	pNewTest->SetTestDuration(sTestTime);

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

		CR(SetupPipeline("minimal"));

		// Test Context
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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

			pTestContext->pCollidePoint[i]->SetMaterialColors(COLOR_BLACK);

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
		pTestContext->pSphereB->translateY(-0.0002f);

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

			//pTestContext->pSphereA->SetVisible(false);
			//pTestContext->pSphereB->SetVisible(false);
		}
		else {

			//pTestContext->pSphereA->SetVisible(true);
			//pTestContext->pSphereB->SetVisible(true);

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
	auto pNewTest = AddTest("spherevssphere", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

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

		CR(SetupPipeline("minimal"));

		// Test Context
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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
	auto pNewTest = AddTest("spherevsquad", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

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

		CR(SetupPipeline("minimal"));

		// Test Context
		TestContext *pTestContext;
		pTestContext = reinterpret_cast<TestContext*>(pContext);
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
	auto pNewTest = AddTest("spherevsobb", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

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

		CR(SetupPipeline("minimal"));

		RayTestContext *pTestContext;
		pTestContext = reinterpret_cast<RayTestContext*>(pContext);
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

		pTestContext->pModel = pTestContext->pComposite->AddModel(L"\\head_01\\head_01.FBX");
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
	auto pNewTest = AddTest("rayvscompositeobjs", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Ray intersection of various objects in a composite and resolving those points");
	pNewTest->SetTestDuration(sTestTime);

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

		CR(SetupPipeline("minimal"));

		RayTestContext *pTestContext;
		pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		CN(pTestContext);

		// Ray to composite

		pTestContext->pComposite = m_pDreamOS->AddComposite();
		CN(pTestContext->pComposite);

		composite *pComposite;
		pComposite = pTestContext->pComposite;
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
	auto pNewTest = AddTest("scaledcompositevsray", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

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

		double yPos = -1.0f;
		double xPos = 2.0f;

		// Ray to quads 
		int quadCount = 0;

		m_pDreamOS->SetGravityState(false);

		CR(SetupPipeline("minimal"));

		RayTestContext *pTestContext;
		pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		CN(pTestContext);

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
	auto pNewTest = AddTest("rayvsmodel", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("Ray intersection of quads oriented in various fashion in a composite");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT CollisionTestSuite::AddTestHysteresisObj() {
	RESULT r = R_PASS;

	double sTestTime = 1000.0f;
	int nRepeats = 1;

	struct TestContext : public Subscriber<HysteresisEvent>, public Subscriber<InteractionObjectEvent> {

		DreamOS *pDreamOS = nullptr;
		volume *pVolumeOff = nullptr;
		volume *pVolumeOn = nullptr;

		HysteresisObject *pObj = nullptr;

		quad *pPointQuad = nullptr;
		sphere *pPointSphereLeft = nullptr;
		sphere *pPointSphereRight = nullptr;

		DimRay *leftRay = nullptr;
		DimRay *rightRay = nullptr;

		virtual RESULT Notify(HysteresisEvent *pEvent) override {
			RESULT r = R_PASS;

			// with DreamUserApp as the Subscriber to the Hysteresis objects, this logic is more reasonable
			sphere *pSphere = nullptr;
			if (pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_LEFT) == pEvent->m_pEventObject) {
				pSphere = pPointSphereLeft;
			}
			else if (pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_RIGHT) == pEvent->m_pEventObject) {
				pSphere = pPointSphereRight;
			}

			CNR(pSphere, R_SKIPPED);
			switch (pEvent->m_eventType) {
			case ON: {
				//pMallet->Show();
				pSphere->SetVisible(true);
			} break;

			case OFF: {
				//pMallet->Hide();
				pSphere->SetVisible(false);
			} break;

			}

		Error:
			return r;
		};

		virtual RESULT Notify(InteractionObjectEvent *pEvent) override {

			auto pUserApp = pDreamOS->GetUserApp();
			auto pLeftHand = pUserApp->GetHand(HAND_TYPE::HAND_LEFT);
			auto pRightHand = pUserApp->GetHand(HAND_TYPE::HAND_RIGHT);
			auto handType = HAND_TYPE::HAND_LEFT;

			if (pEvent->m_pInteractionObject == pLeftHand->GetMalletHead()) {
				handType = HAND_TYPE::HAND_LEFT;
			}
			else if (pEvent->m_pInteractionObject == pRightHand->GetMalletHead()) {
				handType = HAND_TYPE::HAND_RIGHT;
			}

			switch (pEvent->m_eventType) {
			case ELEMENT_INTERSECT_BEGAN: {
				/*
				if (handType == HAND_TYPE::HAND_LEFT) {
					pPointSphereLeft->SetVisible(true);
				}
				else if (handType == HAND_TYPE::HAND_RIGHT) {
					pPointSphereRight->SetVisible(true);
				}
				//*/
			} break;
			case ELEMENT_INTERSECT_MOVED: {

				if (handType == HAND_TYPE::HAND_LEFT) {
					pPointSphereLeft->SetPosition(pEvent->m_ptContact[0]);
				}
				else if (handType == HAND_TYPE::HAND_RIGHT) {
					pPointSphereRight->SetPosition(pEvent->m_ptContact[0]);
				}

			} break;
			case ELEMENT_INTERSECT_ENDED: {
				/*
				if (handType == HAND_TYPE::HAND_LEFT) {
					pPointSphereLeft->SetVisible(false);
				}
				else if (handType == HAND_TYPE::HAND_RIGHT) {
					pPointSphereRight->SetVisible(false);
				}
				//*/
			} break;
			};

			return R_PASS;
		};

		RESULT UpdatePointer(HAND_TYPE handType) {

			auto pUserApp = pDreamOS->GetUserApp();

			auto pHand = pUserApp->GetHand(handType);

			sphere *pPointSphere = nullptr;

			if (handType == HAND_TYPE::HAND_LEFT) {
				pPointSphere = pPointSphereLeft;
			}
			else {
				pPointSphere = pPointSphereRight;
			}

			/*/
			if (pObj->GetState(pHand->GetMalletHead()) == HysteresisEventType::ON) {
				pPointSphere->SetVisible(true);
			}
			else {
				pPointSphere->SetVisible(false);
			}
			//*/

			return R_PASS;
		};

	};
	TestContext *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		CR(SetupPipeline("minimal_texture"));

		CN(m_pDreamOS);
		pTestContext->pDreamOS = m_pDreamOS;

		// cylinder
		pTestContext->pObj = m_pDreamOS->MakeHysteresisObject(0.45f, 0.3f, CYLINDER);

		// sphere
		//pTestContext->pObj = m_pDreamOS->MakeHysteresisObject(0.5f, 0.25f, SPHERE);

		// quad
		//pTestContext->pObj = m_pDreamOS->MakeHysteresisObject(0.5f, 0.25f, PLANE);
		//pTestContext->pObj->RotateXByDeg(-90.0f);

		CN(pTestContext->pObj);

		// reference objects, positioned at the boundaries
		pTestContext->pVolumeOff = m_pDreamOS->AddVolume(0.0125f);
		pTestContext->pVolumeOn = m_pDreamOS->AddVolume(0.0125f);

		pTestContext->pPointQuad = m_pDreamOS->AddQuad(7.844f, 4.412f);
		pTestContext->pPointQuad->RotateXByDeg(90.0f);
		pTestContext->pPointQuad->SetPosition(0.0f, 0.0f, -5.86f);

		pTestContext->pPointSphereLeft = m_pDreamOS->AddSphere(0.025f);
		pTestContext->pPointSphereLeft->SetMaterialDiffuseColor(COLOR_RED);
		pTestContext->pPointSphereRight = m_pDreamOS->AddSphere(0.025f);
		pTestContext->pPointSphereRight->SetMaterialDiffuseColor(COLOR_BLUE);

		pTestContext->leftRay = m_pDreamOS->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -1.0f));
		pTestContext->rightRay = m_pDreamOS->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -1.0f));

		//m_pDreamOS->AddInteractionObject(m_pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_LEFT));
		//m_pDreamOS->AddInteractionObject(m_pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_RIGHT));

		CR(m_pDreamOS->AddAndRegisterInteractionObject(pTestContext->pPointQuad, ELEMENT_INTERSECT_BEGAN, pTestContext));
		CR(m_pDreamOS->AddAndRegisterInteractionObject(pTestContext->pPointQuad, ELEMENT_INTERSECT_MOVED, pTestContext));
		CR(m_pDreamOS->AddAndRegisterInteractionObject(pTestContext->pPointQuad, ELEMENT_INTERSECT_ENDED, pTestContext));

		m_pDreamOS->GetInteractionEngineProxy()->SetInteractionDiffThreshold(0.01f);

		//m_pDreamOS->RegisterpTestContext->pPointQuad

		m_pDreamOS->GetCamera()->SetPosition(0.0f, 0.0f, 1.0f);

		m_pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_LEFT)->SetModelState(hand::ModelState::CONTROLLER);
		m_pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_RIGHT)->SetModelState(hand::ModelState::CONTROLLER);

		m_pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_LEFT)->LoadHandModel();
		m_pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_RIGHT)->LoadHandModel();

		pTestContext->pObj->RegisterObject(m_pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_LEFT));
		pTestContext->pObj->RegisterObject(m_pDreamOS->GetUserApp()->GetHand(HAND_TYPE::HAND_RIGHT));
		pTestContext->pObj->RegisterSubscriber(HysteresisEventType::ON, pTestContext);
		pTestContext->pObj->RegisterSubscriber(HysteresisEventType::OFF, pTestContext);
		

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		auto pTestContext = reinterpret_cast<TestContext*>(pContext);
		//CN(pTestContext);

		pTestContext->pObj->SetPosition(pTestContext->pDreamOS->GetCamera()->GetPosition(true));
		pTestContext->pVolumeOff->SetPosition(pTestContext->pDreamOS->GetCamera()->GetPosition(true) + point(0.0f, -0.1f, -0.25f));
		pTestContext->pVolumeOn->SetPosition(pTestContext->pDreamOS->GetCamera()->GetPosition(true) + point(0.0f, -0.1f, -0.5f));

		pTestContext->pObj->Update();

		pTestContext->UpdatePointer(HAND_TYPE::HAND_LEFT);
		pTestContext->UpdatePointer(HAND_TYPE::HAND_RIGHT);

		/*
		pTestContext->leftRay->UpdateFromRay(pTestContext->pDreamOS->GetHand(HAND_TYPE::HAND_LEFT)->GetMalletHead()->GetRay(true));
		pTestContext->rightRay->UpdateFromRay(pTestContext->pDreamOS->GetHand(HAND_TYPE::HAND_RIGHT)->GetMalletHead()->GetRay(true));
		//pTestContext->leftRay->SetRayVertices(0.1f);
		//pTestContext->rightRay->SetRayVertices(0.1f);
		//*/

		hand *pHand = pTestContext->pDreamOS->GetHand(HAND_TYPE::HAND_LEFT);
		if (pHand->GetModel() != nullptr) {
			point ptMallet = pHand->GetMalletHead()->GetPosition(true);
			ray testRay = ray(ptMallet, vector(ptMallet - pHand->GetPosition(true)).Normal());
			pTestContext->leftRay->UpdateFromRay(testRay);
			pTestContext->leftRay->SetRayVertices(0.1f);
			pTestContext->leftRay->UpdateBuffers();
		}

		pHand = pTestContext->pDreamOS->GetHand(HAND_TYPE::HAND_RIGHT);
		if (pHand->GetModel() != nullptr) {
			pTestContext->rightRay->UpdateFromRay(pHand->GetMalletHead()->GetRay(true));
			pTestContext->rightRay->SetRayVertices(0.1f);
			pTestContext->rightRay->UpdateBuffers();
		}

//		pTestContext->pDreamOS->GetUserApp()->GetMallet(HAND_TYPE::HAND_LEFT)->Show();

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	// Add the test
	auto pNewTest = AddTest("hysteresis", fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestDescription("hysteresis test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}
