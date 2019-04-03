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
#include "Primitives/model/model.h"
#include "Primitives/model/mesh.h"
#include "Primitives/user.h"
#include "Primitives/DimRay.h"
#include "Primitives/DimPlane.h"
#include "Primitives/billboard.h"

#include "Pipeline/Pipeline.h"
#include "Pipeline/PipelineCommon.h"

class SandboxApp;

#include "Primitives/viewport.h"

class SinkNode;
class SourceNode;
class ProgramNode;
struct PrimParams;

//class composite;

class UIKeyboardLayout;

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
	virtual ProgramNode* MakeProgramNode(std::string strNodeName, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE) = 0;

public:

	virtual RESULT Resize(viewport newViewport) = 0;
	virtual RESULT MakeCurrentContext() = 0;
	virtual RESULT ReleaseCurrentContext() = 0;

	FlatProgram* GetFlatProgram();
	virtual RESULT RenderToTexture(FlatContext* pContext, stereocamera* pCamera);
	virtual RESULT RenderToTexture(FlatContext* pContext);

	virtual RESULT Shutdown() = 0;

	virtual RESULT SetViewTarget(EYE_TYPE eye, int pxWidth, int pxHeight) = 0;

	virtual RESULT InitializeHAL() = 0;
	virtual RESULT ClearHALBuffers() = 0;
	virtual RESULT ConfigureHAL() = 0;
	virtual RESULT FlushHALBuffers() = 0;

private:
	RESULT Render();

protected:
	RESULT SetRenderReferenceGeometry(bool fRenderReferenceGeometry);

public:
	bool IsRenderReferenceGeometry();

private:
	template <typename objType>
	class HelperFactory {
		friend class HALImp;

	protected:
		HelperFactory(HALImp *pImp) : m_pImp(pImp) {
			//empty 
		}

		template<typename... Targs>
		objType *TMakeObject(Targs... Fargs);

		HALImp *m_pImp = nullptr;
	};

public:
	template<typename objType, typename... Targs>
	objType* TMakeObject(Targs... Fargs) {
		HelperFactory<objType> helperFactory(this);

		objType *pObj = helperFactory.TMakeObject(Fargs...);

		return pObj;
	}

	template<typename objType>
	objType* TMakeObject() {
		HelperFactory<objType> helperFactory(this);

		objType *pObj = helperFactory.TMakeObject();

		return pObj;
	}

	virtual DimObj *MakeObject(PrimParams *pPrimParams, bool fInitialize = true) = 0;
	virtual RESULT InitializeObject(DimObj *pDimObj) = 0;

	// TODO: Remove and use param pack function
	virtual light* MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) = 0;

	virtual quad* MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector()) = 0;
	virtual quad* MakeQuad(double width, double height, point ptCenter, uvcoord uvTopLeft, uvcoord uvBottomRight, vector vNormal = vector::jVector()) = 0;
	virtual quad* MakeQuad(float width, float height, int numHorizontalDivisions, int numVerticalDivisions, uvcoord uvTopLeft, uvcoord uvBottomRight, quad::CurveType curveType = quad::CurveType::FLAT, vector vNormal = vector::jVector()) = 0;

	virtual sphere* MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE)) = 0;
	virtual cylinder* MakeCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) = 0;
	virtual DimRay* MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) = 0;
	virtual DimPlane* MakePlane(point ptOrigin, vector vNormal) = 0;
	
	virtual volume* MakeVolume(double side, bool fTriangleBased = true) = 0;
	virtual volume* MakeVolume(double width, double length, double height, bool fTriangleBased = true) = 0;

	virtual text *MakeText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags = text::flags::NONE) = 0;
	virtual text *MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM = 0.25f, text::flags textFlags = text::flags::NONE) = 0;
	virtual text *MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 0.25f, text::flags textFlags = text::flags::NONE) = 0;
	virtual text* MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 1.0f, bool fDistanceMap = false, bool fBillboard = false) = 0;
	virtual text* MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& strContent, double width = 1.0f, double height = 1.0f, bool fDistanceMap = false, bool fBillboard = false) = 0;
	virtual text* MakeText(const std::wstring& wstrFontName, const std::string& strContent, double width = 1.0f, double height = 1.0f, bool fDistanceMap = false, bool fBillboard = false) = 0;

	virtual texture* MakeTexture(const texture &srcTexture) = 0;
	virtual texture* MakeTexture(texture::type type, const wchar_t *pszFilename) = 0;
	virtual texture* MakeTexture(texture::type type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n) = 0;
	virtual texture *MakeTextureFromFileBuffer(texture::type type, uint8_t *pBuffer, size_t pBuffer_n) = 0;

	virtual cubemap* MakeCubemap(const std::wstring &wstrCubemapName) = 0;

	virtual skybox *MakeSkybox() = 0;

	//virtual model *MakeModel(wchar_t *pszModelName) = 0;

	virtual mesh *MakeMesh(const std::vector<vertex>& vertices) = 0;
	virtual mesh *MakeMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) = 0;

	//virtual composite* MakeModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation) = 0;

	// TODO: Fix this
	//virtual composite *LoadModel(ObjectStore* pSceneGraph, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, vector vEulerRotation = vector(0.0f, 0.0f, 0.0f)) = 0;

	virtual user *MakeUser() = 0;

	virtual billboard *MakeBillboard(point ptOrigin, float width, float height) = 0;


	// Composite
	//template<typename... Targs>
	composite* TMakeObject() {
		return MakeComposite();
	}

	// TODO: These are the same - should consolidate
	virtual composite *MakeComposite() = 0;
	virtual model *MakeModel() = 0;

	virtual FlatContext* MakeFlatContext(int pxFBWidth, int pxFBHeight, int channels) = 0;

	virtual hand* MakeHand(HAND_TYPE type) = 0;
	virtual hand* MakeHand(HAND_TYPE type, long avatarID) = 0;

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

	bool m_fCurrentContext = false;

private:
	UID m_uid;
};

template<>
template<typename... Targs>
quad* HALImp::HelperFactory<quad>::TMakeObject(Targs... Fargs) {
	return m_pImp->MakeQuad(Fargs...);
}

template<>
template<typename... Targs>
FlatContext* HALImp::HelperFactory<FlatContext>::TMakeObject(Targs... Fargs) {
	return m_pImp->MakeFlatContext(Fargs...);
}

template<>
template<typename... Targs>
texture* HALImp::HelperFactory<texture>::TMakeObject(Targs... Fargs) {
	return m_pImp->MakeTexture(Fargs...);
}


// TODO: a lot of this logic should go into the implementation maybe?
template<>
template<typename... Targs>
text* HALImp::HelperFactory<text>::TMakeObject(Targs... Fargs) {
	RESULT r = R_PASS;

	text *pText = m_pImp->MakeText(Fargs...);
	if (pText != nullptr && pText->IsRenderToQuad()) {
		CR(pText->RenderToQuad());
	}

	return pText;

Error:
	if (pText != nullptr) {
		pText = nullptr;
	}

	return nullptr;
}


#endif // ! HAL_IMP_H_
