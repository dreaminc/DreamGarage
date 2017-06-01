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
class composite;

class HALTestSuite : public TestSuite {
public:
	HALTestSuite(DreamOS *pDreamOS);
	~HALTestSuite();

	virtual RESULT AddTests() override;

public:
	// Note: This should eventually call the DreamGarageApp pipeline
	// function rather than duplicating it
	RESULT SetupSkyboxPipeline();

	RESULT AddTestSkybox();

	RESULT AddTestEnvironmentShader();

	RESULT AddTestText();
	RESULT AddTestModel();
	RESULT AddTestModelOrientation();
	RESULT AddTestQuadObject();
	RESULT AddTestUIShaderStage();

	RESULT TestNestedOBB();

	// TODO: Consolidate the HMD tests 
	// TODO: This should be easy as this is now supported by pipeline
	RESULT AddTestMinimalShaderHMD();
	RESULT AddTestBlinnPhongShader();
	RESULT AddTestBlinnPhongShaderBlurHMD();
	RESULT AddTestBlinnPhongShaderTextureHMD();
	RESULT AddTestBlinnPhongShaderTextureCopy();

	RESULT AddTestMouseDrag();
	RESULT AddTestBlinnPhongShadowShader();

	RESULT AddTestMinimalShader();
	RESULT AddTestMinimalTextureShader();
	RESULT AddTestBlinnPhongShaderBlur();
	RESULT AddTestBlinnPhongShaderTexture();

	RESULT AddTestSenseHaptics();

	RESULT AddTestDepthPeelingShader();

	RESULT AddTestRenderToTextureQuad();
	RESULT AddTestFramerateVolumes();
	RESULT AddTestAlphaVolumes();

private:
	RESULT ResetTest(void *pContext);

private:
	HALImp *GetHALImp();

private:
	DreamOS *m_pDreamOS;
	HALImp *m_pHALImp = nullptr;

	composite *m_pModel = nullptr;
};

#endif // ! HAL_TEST_SUITE_H_
