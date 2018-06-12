#ifndef HAL_TEST_SUITE_H_
#define HAL_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/HALTestSuite.h

// This test suite should cover testing all functionality related to the
// HAL (graphics) layer and interfaces.

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;
class HALImp;

class HALTestSuite : public TestSuite {
public:
	HALTestSuite(DreamOS *pDreamOS);
	~HALTestSuite();

	virtual RESULT AddTests() override;

public:
	// Note: This should eventually call the DreamGarageApp pipeline
	// function rather than duplicating it
	RESULT SetupSkyboxPipeline(std::string strRenderShaderName = "environment");

	RESULT AddTestSkybox();

	// Shader Tests
	RESULT AddTestGeometryShader();

	// Environments
	RESULT AddTestEnvironmentShader();
	RESULT AddTestEnvironments();

	// Objects
	RESULT AddTestModelInstancing();
	RESULT AddTestText();
	RESULT AddTestModel();
	RESULT AddTestUserModel();
	RESULT AddTestModelOrientation();
	RESULT AddTestQuadObject();
	RESULT AddTestUIShaderStage();
	RESULT AddTestFlatContextNesting();

	RESULT TestNestedOBB();

	RESULT AddTestRotation();

	// TODO: Consolidate the HMD tests 
	// TODO: This should be easy as this is now supported by pipeline
	RESULT AddTestMinimalShaderHMD();
	RESULT AddTestBlinnPhongShader();
	RESULT AddTestBlinnPhongShaderBlurHMD();
	RESULT AddTestBlinnPhongShaderTextureHMD();
	RESULT AddTestBlinnPhongShaderTextureCopy();

	RESULT AddTestMouseDrag();
	RESULT AddTestMinimalShader();
	RESULT AddTestMinimalTextureShader();
	RESULT AddTestBlinnPhongShaderBlur();
	RESULT AddTestBlinnPhongShaderTexture();

	RESULT AddTestSenseHaptics();

	RESULT AddTestDepthPeelingShader();

	RESULT AddTestRenderToTextureQuad();
	RESULT AddTestFramerateVolumes();
	RESULT AddTestAlphaVolumes();

	RESULT AddTestRemoveObjects();

private:
	RESULT ResetTest(void *pContext);

private:
	HALImp *GetHALImp();

private:
	DreamOS *m_pDreamOS;
	HALImp *m_pHALImp = nullptr;
};

#endif // ! HAL_TEST_SUITE_H_
