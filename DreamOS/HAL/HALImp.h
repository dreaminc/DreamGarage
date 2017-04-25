#ifndef HAL_IMP_H_
#define HAL_IMP_H_
#include "./RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/Subscriber.h"

// Dream OS
// DreamOS/HAL/HALImp.h
// The HAL Implementation class  is the parent class for implementations
// such as the OpenGL implementation and ultimately native ones as well

#include "Scene/ObjectStore.h"

#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"

#include "Primitives/valid.h"

#include "HMD/HMD.h"

#include "Primitives/stereocamera.h"
#include "Primitives/light.h"
#include "Primitives/quad.h"
#include "Primitives/FlatContext.h"
#include "Primitives/sphere.h"
#include "Primitives/cylinder.h"
#include "Primitives/volume.h"
#include "Primitives/text.h"
#include "Primitives/skybox.h"
#include "Primitives/model.h"
#include "Primitives/user.h"
#include "Primitives/DimRay.h"

#include "Pipeline/Pipeline.h"

class SandboxApp;

#include "Primitives/viewport.h"

class SinkNode;
class SourceNode;
class ProgramNode;

class FlatProgram;	// This one is special for render to texture

class HALImp : public valid 
{
	friend class SandboxApp;

public:
	struct HALConfiguration {
		unsigned fRenderReferenceGeometry : 1;
		unsigned fDrawWireframe : 1;
		unsigned fRenderProfiler : 1;
	};

protected:
	HALConfiguration m_HALConfiguration;

public:
	RESULT SetDrawWireframe(bool fDrawWireframe);
	bool IsDrawWireframe();
	RESULT SetRenderProfiler(bool fRenderProfiler);
	bool IsRenderProfiler();

public:
	HALImp();
	~HALImp();

	RESULT SetHALConfiguration(HALConfiguration halconf);
	const HALImp::HALConfiguration& GetHALConfiguration();

public:
	stereocamera* GetCamera();
	RESULT SetCamera(stereocamera* pCamera);

	RESULT SetCameraOrientation(quaternion qOrientation);
	RESULT SetCameraPositionDeviation(vector vDeviation);

	RESULT SetHMD(HMD *pHMD);

	RESULT SetViewport(const viewport &newViewport);
	RESULT SetViewport(int pxWidth, int pxHeight);
	const viewport& GetViewport();

public:
	RESULT InitializeRenderPipeline();
	Pipeline* GetRenderPipelineHandle() {
		return m_pRenderPipeline.get();
	}
	
	virtual SinkNode* MakeSinkNode(std::string strSinkNodeName) = 0;
	virtual SourceNode* MakeSourceNode(std::string strNodeName) = 0;
	virtual ProgramNode* MakeProgramNode(std::string strNodeName) = 0;

public:

	virtual RESULT Resize(viewport newViewport) = 0;
	virtual RESULT MakeCurrentContext() = 0;
	virtual RESULT ReleaseCurrentContext() = 0;

	virtual RESULT RenderToTexture(FlatContext* pContext, stereocamera* pCamera);

	virtual RESULT Shutdown() = 0;

	virtual RESULT SetViewTarget(EYE_TYPE eye, int pxWidth, int pxHeight) = 0;

	virtual RESULT InitializeHAL() = 0;
	virtual RESULT ClearHALBuffers() = 0;
	virtual RESULT ConfigureHAL() = 0;
	virtual RESULT FlushHALBuffers() = 0;

private:
	RESULT Render(ObjectStore* pSceneGraph, stereocamera* pCamera, EYE_TYPE eye);

protected:
	RESULT SetRenderReferenceGeometry(bool fRenderReferenceGeometry);

public:
	bool IsRenderReferenceGeometry();

public:
	// TODO: Remove and use param pack fn
	virtual light* MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) = 0;
	virtual quad* MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector()) = 0;
	virtual quad* MakeQuad(double width, double height, point origin, vector vNormal = vector::jVector()) = 0;

	virtual sphere* MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE)) = 0;
	virtual cylinder* MakeCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) = 0;
	virtual DimRay* MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) = 0;
	
	virtual volume* MakeVolume(double side, bool fTriangleBased = true) = 0;
	virtual volume* MakeVolume(double width, double length, double height, bool fTriangleBased = true) = 0;

	virtual text* MakeText(const std::wstring& fontName, const std::string& content, double size = 1.0f, bool fDistanceMap = false, bool isBillboard = false) = 0;
	virtual text* MakeText(std::shared_ptr<Font> pFont, const std::string& content, double size = 1.0f, bool fDistanceMap = false, bool isBillboard = false) = 0;
	virtual text* MakeText(std::shared_ptr<Font> pFont, texture *pFontTexture, const std::string& content, double size = 1.0f, bool fDistanceMap = false, bool isBillboard = false) = 0;

	virtual texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) = 0;
	virtual texture* MakeTexture(texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n) = 0;
	virtual texture *MakeTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n, texture::TEXTURE_TYPE type) = 0;

	virtual skybox *MakeSkybox() = 0;
	virtual model *MakeModel(wchar_t *pszModelName) = 0;
	virtual model *MakeModel(const std::vector<vertex>& vertices) = 0;
	virtual model *MakeModel(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) = 0;
	virtual composite* MakeModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation) = 0;

	// TODO: Fix this
	virtual composite *LoadModel(ObjectStore* pSceneGraph, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, vector vEulerRotation = vector(0.0f, 0.0f, 0.0f)) = 0;

	virtual user *MakeUser() = 0;

	virtual composite *MakeComposite() = 0;
	virtual FlatContext* MakeFlatContext(int width, int height, int channels) = 0;
	virtual hand* MakeHand() = 0;

	/*
	virtual model* MakeModel(const std::vector<vertex>& vertices) = 0;
	*/

protected:	
	HMD *m_pHMD;
	stereocamera* m_pCamera = nullptr;
	viewport m_viewport;

protected:
	std::unique_ptr<Pipeline> m_pRenderPipeline = nullptr;
	
	// This is used to render to texture
	ProgramNode* m_pFlatProgram = nullptr;

private:
	UID m_uid;
};

#endif // ! HAL_IMP_H_
