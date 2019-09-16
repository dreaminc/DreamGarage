#ifndef OPEN_GL_IMP_H_
#define OPEN_GL_IMP_H_

#include "core/ehm/EHM.h"

// Dream HAL Vulkan
// dos/src/hal/vulkan/VulkanImp.h

// This is the top level header for OpenGL for either native or

#include "hal/HALImp.h"

#include "core/types/version.h"

#include "core/dimension/DimObj.h"
#include "core/material/material.h"

// TODO: 
//#include "OGLProgramFactory.h"

#include <memory>

class Sandbox; 
class Windows64App;
class font;
class mesh;

class VulkanImp : public HALImp {
private:
	version m_versionVulkan;

public:
	VulkanImp();
	~VulkanImp();

	virtual std::string GetHALName() {
		return "vulkan";
	}

	// Object Factory Methods

public:
	version GetVulkanVersion() { return m_versionVulkan; }

	RESULT CheckVulkanError();

	//virtual RESULT InitializeObject(DimObj *pDimObj) override;
	//virtual RESULT InitializeTexture(texture *pTexture) override;

	//virtual DimObj* MakeObject(PrimParams *pPrimParams, bool fInitialize = true) override;
	//virtual texture* MakeTexture(PrimParams *pPrimParams, bool fInitialize = true) override;

	// TODO: Remove and use param pack function
	//virtual light* MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) override;
	//
	//VulkanQuad* MakeQuad(PrimParams *pPrimParams, bool fInitialize);
	//virtual quad* MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector()) override;
	//virtual quad* MakeQuad(double width, double height, point ptCenter, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal = vector::jVector()) override;
	//virtual quad* MakeQuad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType = quad::CurveType::FLAT, vector vNormal = vector::jVector()) override;
	//
	//VulkanSphere* MakeSphere(PrimParams *pSphereParams, bool fInitialize = false);
	//virtual sphere* MakeSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) override;
	//
	//virtual cylinder* MakeCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) override;
	//virtual DimRay* MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) override;
	//virtual DimPlane* MakePlane(point ptOrigin = point(), vector vNormal = vector::jVector(1.0f)) override;
	//
	//VulkanVolume *MakeVolume(PrimParams *pPrimParams, bool fInitialize = false);
	//virtual volume* MakeVolume(double side, bool fTriangleBased = true) override;
	//virtual volume* MakeVolume(double width, double length, double height, bool fTriangleBased = true) override;
	//
	//virtual text *MakeText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags = text::flags::NONE) override;
	//virtual text *MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM = 0.25f, text::flags textFlags = text::flags::NONE) override;
	//virtual text *MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 0.25f, text::flags textFlags = text::flags::NONE) override;
	//virtual text* MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strContent, double width = 1.0f, double height = 1.0f, bool fDistanceMap = false, bool fBillboard = false) override;
	//virtual text* MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 1.0f, bool fDistanceMap = false, bool fBillboard = false) override;
	//virtual text* MakeText(const std::wstring& wstrFontName, const std::string& strContent, double width = 1.0f, double height = 1.0f, bool fDistanceMap = false, bool fBillboard = false) override;
	//
	//virtual texture* MakeTexture(const texture &srcTexture) override;
	//virtual texture* MakeTexture(texture::type type, const wchar_t *pszFilename) override;
	//virtual texture* MakeTexture(texture::type type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n) override;
	//virtual texture* MakeTextureFromFileBuffer(texture::type type, uint8_t *pBuffer, size_t pBuffer_n) override;
	//
	//virtual cubemap* MakeCubemap(const std::wstring &wstrCubemapName) override;
	//
	//skybox *MakeSkybox();
	//
	////mesh *MakeMesh(wchar_t *pszModelName);
	//VulkanMesh* MakeMesh(PrimParams *pPrimParams, bool fInitialize = false);
	//mesh *MakeMesh(const std::vector<vertex>& vertices);
	//mesh *MakeMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);
	//
	//VulkanModel* MakeModel(PrimParams *pPrimParams, bool fInitialize = false);
	//virtual composite *MakeComposite() override;
	//virtual model *MakeModel() override;
	//
	//FlatContext* MakeFlatContext(int width, int height, int channels);
	//user *MakeUser();
	//
	//virtual billboard *MakeBillboard(point ptOrigin, float width, float height) override;
	//
	//hand* MakeHand(HAND_TYPE type);
	//hand* MakeHand(HAND_TYPE type, long avatarID);
	//
	//// TODO: Fix w/ scene graph not here
	////composite *LoadModel(ObjectStore* pSceneGraph, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, vector vEulerRotation = vector(0.0f, 0.0f, 0.0f));

public:
	virtual RESULT SetViewTarget(EYE_TYPE eye, int pxWidth, int pxHeight) override;
	virtual RESULT ClearHALBuffers() override;
	virtual RESULT ConfigureHAL() override;
	virtual RESULT FlushHALBuffers() override;

	virtual SinkNode* MakeSinkNode(std::string strSinkNodeName) override;
	virtual SourceNode* MakeSourceNode(std::string strNodeName) override;
	virtual ProgramNode* MakeProgramNode(std::string strNodeName, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE) override;

public:
	virtual RESULT Resize(viewport newViewport) override;
	virtual RESULT Shutdown() override;
	virtual RESULT InitializeHAL() override;

	// Rendering Context 
	virtual RESULT MakeCurrentContext() override ;
	virtual RESULT ReleaseCurrentContext() override;

public:
	
};

#endif // ! OPEN_GL_IMP_H
