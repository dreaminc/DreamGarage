#include "DimensionTestSuite.h"

#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "DreamGarage\DreamGamepadCameraApp.h"

DimensionTestSuite::DimensionTestSuite(DreamOS *pDreamOS) :
	DreamTestSuite("dimension", pDreamOS)
{
	// empty
}

RESULT DimensionTestSuite::AddTests() {
	RESULT r = R_PASS;

	// Add the tests

	CR(AddTestRotation());

Error:
	return r;
}

RESULT DimensionTestSuite::SetupTestSuite() {
	RESULT r = R_PASS;

	// empty

Error:
	return r;
}

RESULT DimensionTestSuite::AddTestRotation() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.5f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		CR(SetupPipeline("minimal"));

		// Objects 

		DimRay *pRay;
		pRay = nullptr;

		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(-1.0f);

			pRay->RotateZByDeg(45.0f * i);
		}

		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(0.0f);

			pRay->RotateZBy(M_PI_4 * i);
		}

		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(1.0f);

			pRay->SetRotate(0.0f, 0.0f, (M_PI_4 * i));
		}

		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(2.0f);

			pRay->SetRotateDeg(0.0f, 0.0f, (45.0f * i));
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


