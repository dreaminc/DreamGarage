#include "HALTestSuite.h"
#include "DreamOS.h"

HALTestSuite::HALTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

HALTestSuite::~HALTestSuite() {
	// empty
}

RESULT HALTestSuite::AddTests() {
	RESULT r = R_PASS;
	
	CR(AddTestAlphaVolumes());

	CR(AddTestRenderToTextureQuad());

	CR(AddTestFramerateVolumes());

Error:
	return r;
}

RESULT HALTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

Error:
	return r;
}


RESULT HALTestSuite::AddTestRenderToTextureQuad() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.3f;
	float alpha = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		{

			composite *pComposite = m_pDreamOS->AddComposite();
			CN(pComposite);

			std::shared_ptr<FlatContext> pFlatContext = pComposite->MakeFlatContext();
			CN(pFlatContext);

			std::shared_ptr<quad> pFlatQuad = pFlatContext->AddQuad(1.0f, 1.0f, point(0.0f));
			CN(pContext);

			pComposite->RenderToTexture(pFlatContext);

			quad *pQuad = m_pDreamOS->AddQuad(width, height);
			CN(pQuad);
			CN(pQuad->SetPosition(point(0.0f, -2.0f, 0.0f)));
			//pQuad->SetColor(COLOR_GREEN);
			CR(pQuad->SetColorTexture(pFlatContext->GetFramebuffer()->GetTexture()));
			
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Render To Texture");
	pNewTest->SetTestDescription("Testing rendering to texture using a quad");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestAlphaVolumes() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.3f;
	float alpha = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		volume *pVolume = nullptr;

		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -3.0f));
		CR(pVolume->SetColor(COLOR_GREEN));
		CR(pVolume->SetAlpha(alpha));


		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * 0.0f));
		CR(pVolume->SetColor(COLOR_WHITE));
		CR(pVolume->SetAlpha(alpha));
		
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -1.0f));
		CR(pVolume->SetColor(COLOR_RED));
		CR(pVolume->SetAlpha(alpha));
		
		pVolume = m_pDreamOS->AddVolume(width, height, length);
		CN(pVolume);
		pVolume->SetPosition(point(-width, 0.0f, (length + padding) * -2.0f));
		CR(pVolume->SetColor(COLOR_BLUE));
		CR(pVolume->SetAlpha(alpha));
	

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {		
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Alpha Volumes");
	pNewTest->SetTestDescription("Test alpha with volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT HALTestSuite::AddTestFramerateVolumes() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 0.25f;
	float height = width;
	float length = width;

	float padding = 0.2f;

	int numObj = 7;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		m_pDreamOS->SetGravityState(false);

		volume *pVolume = nullptr;  
		
		for (int i = 0; i < numObj; i++) {
			for (int j = 0; j < numObj; j++) {
				for (int k = 0; k < numObj; k++) {
					float xPos = (-1.0f * ((float)numObj / 2.0f)) * (width + padding) + (i * (width + padding));
					float yPos = (-1.0f * ((float)numObj / 2.0f)) * (width + padding) + (j * (width + padding));
					float zPos = (-1.0f * ((float)numObj / 2.0f)) * (width + padding) + (k * (width + padding));
					
					pVolume = m_pDreamOS->AddVolume(width, height, length);
					pVolume->SetPosition(point(xPos, yPos, zPos));
				}
			}
		}

		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Framerate Volumes");
	pNewTest->SetTestDescription("Test frame rate vs many volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

HALImp* HALTestSuite::GetHALImp() {
	return m_pDreamOS->GetHALImp();
}