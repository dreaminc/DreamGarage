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

#include "DreamOS.h"

class HALImp;

class HALTestSuite : public TestSuite {
public:
	HALTestSuite(DreamOS *pDreamOS);
	~HALTestSuite();

	virtual RESULT AddTests() override;

public:
	// Note: This should eventually call the DreamGarageApp pipeline
	// function rather than duplicating it
	RESULT SetupSkyboxPipeline(std::string strRenderShaderName = "standard");

	RESULT AddTestSkybox();

	// Shader Tests
	RESULT AddTestFadeShader();
	RESULT AddTestWaterShader();
	RESULT AddTestWaterShaderCube();
	RESULT AddTestGeometryShader();
	RESULT AddTestIncludeShader();
	RESULT AddTestToonShader();
	RESULT AddTestBlinnPhongShader();
	RESULT AddTestBlinnPhongShaderTexture();
	RESULT AddTestBlinnPhongShaderTextureBump();
	RESULT AddTestBlinnPhongShaderTextureBumpDisplacement();
	RESULT AddTestMinimalShader();
	RESULT AddTestMinimalTextureShader();
	RESULT AddTestBlinnPhongShaderBlur();
	RESULT AddTestDepthPeelingShader();
	RESULT AddTestObjectMaterialsBump();
	RESULT AddTestObjectMaterialsColors();
	RESULT AddTestEnvironmentMapping();

	// Pixel Buffer Object
	RESULT AddTestTextureUpdate();
	RESULT AddTestPBOTextureUpload();
	RESULT AddTestPBOTextureReadback();

	// Environments
	RESULT AddTestStandardShader();
	RESULT AddTestEnvironments();

	// Objects
	RESULT AddTestModelInstancing();
	RESULT AddTestText();
	RESULT AddTestModel();
	RESULT AddTestUserModel();
	RESULT AddTestModelOrientation();
	RESULT AddTestQuadObject();
	RESULT AddTestHeightQuadObject();
	RESULT AddTestUIShaderStage();
	RESULT AddTestFlatContextNesting();

	RESULT AddTestCubeMap();
	RESULT AddTestIrradianceMap();

	RESULT TestNestedOBB();
	RESULT AddTestRotation();

	RESULT AddTestMouseDrag();
	
	RESULT AddTestSenseHaptics();

	RESULT AddTestRenderToTextureQuad();
	RESULT AddTestFramerateVolumes();
	RESULT AddTestAlphaVolumes();

	RESULT AddTestRemoveObjects();

	RESULT AddTestCamera();
	RESULT AddTest3rdPersonCamera();

private:
	RESULT ResetTest(void *pContext);

private:
	HALImp* GetHALImp() {
		return m_pDreamOS->GetHALImp();
	}

private:
	DreamOS *m_pDreamOS = nullptr;
	HALImp *m_pHALImp = nullptr;
};

#endif // ! HAL_TEST_SUITE_H_
