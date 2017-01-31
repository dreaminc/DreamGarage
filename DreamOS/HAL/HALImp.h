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

class SandboxApp;

class HALImp : public Subscriber<SenseKeyboardEvent>, public Subscriber<SenseMouseEvent>, public valid {
private:
	struct HALConfiguration {
		unsigned fRenderReferenceGeometry : 1;
	} m_HALConfiguration;

public:
	HALImp();
	~HALImp();

	friend class SandboxApp;

public:
	camera *GetCamera();
	RESULT SetCameraOrientation(quaternion qOrientation);
	RESULT SetCameraPositionDeviation(vector vDeviation);

	RESULT SetHMD(HMD *pHMD);
public:

	virtual RESULT Resize(int pxWidth, int pxHeight) = 0;
	virtual RESULT MakeCurrentContext() = 0;

	virtual RESULT Render(ObjectStore* pSceneGraph, ObjectStore* pFlatSceneGraph, EYE_TYPE eye) = 0;
	virtual RESULT RenderToTexture(FlatContext* pContext) = 0;

	virtual RESULT Shutdown() = 0;

protected:
	RESULT SetRenderReferenceGeometry(bool fRenderReferenceGeometry);
	bool IsRenderReferenceGeometry();

public:
	virtual light* MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) = 0;
	virtual quad* MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector()) = 0;
	virtual quad* MakeQuad(double width, double height, point origin, vector vNormal = vector::jVector()) = 0;

	virtual sphere* MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE)) = 0;
	virtual cylinder* MakeCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) = 0;
	virtual DimRay* MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) = 0;
	
	virtual volume* MakeVolume(double side, bool fTriangleBased = true) = 0;
	virtual volume* MakeVolume(double width, double length, double height, bool fTriangleBased = true) = 0;

	virtual text* MakeText(const std::wstring& fontName, const std::string& content, double size = 1.0f, bool fDistanceMap = false, bool isBillboard = false) = 0;
	virtual texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) = 0;
	virtual texture* MakeTexture(texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n) = 0;
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
	stereocamera *m_pCamera;
	HMD *m_pHMD;

private:
	UID m_uid;
};

#endif // ! HAL_IMP_H_
