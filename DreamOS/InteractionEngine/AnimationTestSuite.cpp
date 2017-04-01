#include "AnimationTestSuite.h"
#include "AnimationQueue.h"

#include "DreamOS.h"

AnimationTestSuite::AnimationTestSuite(DreamOS *pDreamOS) {
	// empty
	m_pDreamOS = pDreamOS;
}

AnimationTestSuite::~AnimationTestSuite() {
	// empty
}

RESULT AnimationTestSuite::AddTests() {
	RESULT r = R_PASS;
	CR(AddTestAnimation());
Error:
	return r;
}

RESULT AnimationTestSuite::AddTestAnimation() {

	RESULT r = R_PASS;

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		composite *pComposite = nullptr;
		std::shared_ptr<sphere> pSphere = nullptr;
		AnimationQueue *pQueue = nullptr;
		AnimationState aState;

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);
		pComposite->InitializeOBB();

		pSphere = pComposite->AddSphere(1.0f, 10.0f, 10.0f);
		pSphere->MoveTo(0.0f, 0.0f, -2.0f);
		CN(pSphere);

		// may not be needed
		CR(pSphere->InitializeOBB());

		CR(m_pDreamOS->AddInteractionObject(pComposite));
		pQueue = m_pDreamOS->GetAnimationQueue();

		aState.ptPosition = point(0.0f, 1.0f, -2.0f);
		aState.vScale = vector(5.0f, 1.25f, 1.25f);

		pQueue->PushAnimationItem(pSphere.get(), aState, 10.0f);


	Error:
		return R_PASS;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;
		CR(r);
	Error:
		return R_PASS;
	};

	auto pNewTest = AddTest(fnInitialize, fnUpdate, nullptr);
	CN(pNewTest);

	pNewTest->SetTestName("Ray Events Controller Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(10000.0);
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

