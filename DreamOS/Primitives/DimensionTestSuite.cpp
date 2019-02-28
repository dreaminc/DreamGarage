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
	CR(AddTestVectorRotationFunctions());
	CR(AddTestDimObjRotationFunctions());

Error:
	return r;
}

RESULT DimensionTestSuite::SetupTestSuite() {
	RESULT r = R_PASS;

	// empty

Error:
	return r;
}

RESULT DimensionTestSuite::AddTestDimObjRotationFunctions() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.75f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		float lastY = -1.0f;
		DimRay *pRay = nullptr;

		CR(SetupPipeline("minimal"));

		// Objects 

		// Rotate by Deg
		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(lastY);

			pRay->RotateZByDeg(45.0f * i);
		}
		lastY += padding;

		// Rotate by
		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(lastY);

			pRay->RotateZBy(M_PI_4 * i);
		}
		lastY += padding;

		// Set Rotate
		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(lastY);

			pRay->SetRotate(0.0f, 0.0f, (M_PI_4 * i));
		}
		lastY += padding;

		// Set Rotate Deg
		for (int i = 0; i < 9; i++) {
			pRay = m_pDreamOS->AddRay(point(0.0f), vector::iVector(1.0f), 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * 1.0f));
			pRay->translateY(lastY);

			pRay->SetRotateDeg(0.0f, 0.0f, (45.0f * i));
		}
		lastY += padding;

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
	auto pNewTest = AddTest("dimobjrotation", fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestDescription("Confirm consistency of DimObj rotation functions");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT DimensionTestSuite::AddTestVectorRotationFunctions() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 1.5f;
	float height = width;
	float length = width;

	float padding = 0.75f;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		float lastY = -1.0f;
		DimRay *pRay = nullptr;

		CR(SetupPipeline("minimal"));

		// Objects 

		// Rotate Points with Rotation Matrix to Vector
		// This works
		for (int i = 0; i < 9; i++) {
			quaternion qRotation = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, (M_PI_4 * i));

			RotationMatrix rotMat = RotationMatrix(qRotation);
			vector vVec = (vector)(rotMat * vector::jVector(1.0f));

			pRay = m_pDreamOS->AddRay(point(0.0f), vVec, 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * padding));
			pRay->translateY(lastY);
		}
		lastY += padding;

		// Rotate Points with Rotate by Quaternion
		// This doesn't work
		for (int i = 0; i < 9; i++) {
			quaternion qRotation = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, (M_PI_4 * i));
			vector vVec = (vector::jVector(1.0f)).RotateByQuaternion(qRotation);

			pRay = m_pDreamOS->AddRay(point(0.0f), vVec, 0.5f);
			CN(pRay);
			pRay->translateX(-4.5f + (i * padding));
			pRay->translateY(lastY);
		}
		lastY += padding;

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
	auto pNewTest = AddTest("vectorrotationfunctions", fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestDescription("Confirm consistency of DimObj rotation functions");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}


