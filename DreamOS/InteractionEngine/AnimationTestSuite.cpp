#include "AnimationTestSuite.h"
#include "AnimationQueue.h"

#include "DreamOS.h"
#include "Primitives/composite.h"
#include "Primitives/sphere.h"

AnimationTestSuite::AnimationTestSuite(DreamOS *pDreamOS) {
	// empty
	m_pDreamOS = pDreamOS;
}

AnimationTestSuite::~AnimationTestSuite() {
	// empty
}

RESULT AnimationTestSuite::AddTests() {
	RESULT r = R_PASS;
	CR(AddTestAnimationBasic());
	CR(AddTestCancel());
Error:
	return r;
}

RESULT AnimationTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

Error:
	return r;
}

RESULT AnimationTestSuite::InitializeAnimationTest(void *pContext) {
	RESULT r = R_PASS;

	AnimationTestContext *pTestContext = reinterpret_cast<AnimationTestContext*>(pContext);

	pTestContext->pComposite = m_pDreamOS->AddComposite();
	CN(pTestContext->pComposite);
	pTestContext->pComposite->InitializeOBB();

	pTestContext->pSphere = m_pDreamOS->AddSphere(1.0f, 10.0f, 10.0f);
	pTestContext->pSphere->MoveTo(0.0f, 0.0f, -2.0f);
	CN(pTestContext->pSphere);

	CR(pTestContext->pSphere->InitializeOBB());

	pTestContext->pSphere2 = m_pDreamOS->AddSphere(1.0f, 10.0f, 10.0f);
	pTestContext->pSphere2->MoveTo(2.0f, 0.0f, - 2.0f);
	CN(pTestContext->pSphere2);

	CR(pTestContext->pSphere2->InitializeOBB());

	pTestContext->fCancelled = false;

	pTestContext->startTime = std::chrono::high_resolution_clock::now();

Error:
	return r;
}

RESULT AnimationTestSuite::AddTestAnimationBasic() {

	RESULT r = R_PASS;

	AnimationTestContext *pTestContext = new AnimationTestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		AnimationQueue *pQueue = nullptr;
		AnimationState aState;
		AnimationState aState2;

		CR(InitializeAnimationTest(pContext));
		AnimationTestContext *pTestContext = reinterpret_cast<AnimationTestContext*>(pContext);

		CR(m_pDreamOS->AddInteractionObject(pTestContext->pComposite));
		pQueue = m_pDreamOS->GetAnimationQueue();

		aState.ptPosition = point(0.0f, 1.0f, -2.0f);
		aState.vScale = vector(5.0f, 1.25f, 1.25f);

		pQueue->PushAnimationItem(pTestContext->pSphere, aState, 2.0f);
		
		aState2.ptPosition = point(1.0f, 0.0f, 0.0f);
		aState2.vScale = vector(1.0f, 1.0f, 1.0f);
		pQueue->PushAnimationItem(pTestContext->pSphere, aState2, 2.0f);

		aState2.ptPosition = point(2.0f, 1.0f, -2.0f);
		aState2.vScale = vector(0.5f, 0.5f, 0.5f);

		pQueue->PushAnimationItem(pTestContext->pSphere2, aState2, 5.0f);

	Error:
		return R_PASS;
	};

	auto fnTest = [&](void* pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	auto pNewTest = AddTest(fnInitialize, fnTest, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray Events Controller Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(10.0);
	pNewTest->SetTestRepeats(1);
Error:
	return r;
}

RESULT AnimationTestSuite::AddTestCancel() {
	RESULT r = R_PASS;

	AnimationTestContext *pTestContext = new AnimationTestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		AnimationQueue *pQueue = nullptr;
		AnimationState aState;
		AnimationState aState2;

		CR(InitializeAnimationTest(pContext));
		AnimationTestContext *pTestContext = reinterpret_cast<AnimationTestContext*>(pContext);

		CR(m_pDreamOS->AddInteractionObject(pTestContext->pComposite));
		pQueue = m_pDreamOS->GetAnimationQueue();

		aState.ptPosition = point(0.0f, 1.0f, -2.0f);
		aState.vScale = vector(1.25f, 1.25f, 1.25f);

		pQueue->PushAnimationItem(pTestContext->pSphere, aState, 2.0f);
		//m_pDreamOS->GetAnimationQueue()->CancelAnimation(pTestContext->pSphere);

	Error:
		return R_PASS;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		AnimationTestContext* pTestContext = reinterpret_cast<AnimationTestContext*>(pContext);
		CN(pTestContext);

		auto diff = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - pTestContext->startTime).count();

		if (diff > 1.0 && !pTestContext->fCancelled) {
			m_pDreamOS->GetAnimationQueue()->CancelAnimation(pTestContext->pSphere);
			pTestContext->fCancelled = true;
		}

	Error:
		return R_PASS;
	};

	auto fnTest = [&](void* pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray Events Controller Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(10.0);
	pNewTest->SetTestRepeats(1);
Error:
	return r;

}

RESULT AnimationTestSuite::Notify(InteractionObjectEvent *event) {
	return R_PASS;
}

RESULT AnimationTestSuite::Notify(SenseKeyboardEvent *kbEvent) {
	return R_PASS;
}
