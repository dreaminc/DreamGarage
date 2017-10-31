#include "CollisionTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "PhysicsEngine/CollisionManifold.h"


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

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
	//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

	light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECITONAL, 2.5f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));

Error:
	return r;
}

RESULT CollisionTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

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
			pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
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
			pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
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
				pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetMaterialDiffuseColor(color(COLOR_BLUE));
				//pTestContext->pCollidePoint[i]->SetColor(color(COLOR_BLUE));
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
