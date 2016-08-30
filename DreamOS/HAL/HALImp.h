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
#include "Primitives/sphere.h"
#include "Primitives/volume.h"
#include "Primitives/text.h"
#include "Primitives/skybox.h"
#include "Primitives/model.h"
#include "Primitives/user.h"

class HALImp : public Subscriber<SenseKeyboardEvent>, public Subscriber<SenseMouseEvent>, public valid {
public:
	HALImp();
	~HALImp();

public:
	camera *GetCamera();
	RESULT UpdateCamera();
	RESULT SetCameraOrientation(quaternion qOrientation);
	RESULT SetCameraPositionDeviation(vector vDeviation);

	RESULT SetHMD(HMD *pHMD);
public:

	virtual RESULT Resize(int pxWidth, int pxHeight) = 0;
	virtual RESULT MakeCurrentContext() = 0;

	virtual RESULT Render(ObjectStore *pSceneGraph) = 0;
	virtual RESULT RenderFlat(ObjectStore *pFlatSceneGraph) = 0;

	virtual RESULT RenderStereo(ObjectStore *pSceneGraph) = 0;
	virtual RESULT RenderStereoFramebuffers(ObjectStore *pSceneGraph) = 0;
	virtual RESULT RenderStereoFramebuffersFlat(ObjectStore *pFlatSceneGraph) = 0;

	virtual RESULT RenderFlush() = 0;

	virtual RESULT Shutdown() = 0;

public:
	virtual light* MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) = 0;
	virtual quad* MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr) = 0;

	virtual sphere* MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE)) = 0;
	virtual volume* MakeVolume(double width, double length, double height) = 0;

	virtual volume* MakeVolume(double side) = 0;
	virtual text* MakeText(const std::wstring& fontName, const std::string& content, double size = 1.0f, bool isBillboard = false) = 0;
	virtual texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) = 0;
	virtual texture* MakeTexture(texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n) = 0;
	virtual skybox *MakeSkybox() = 0;
	virtual model *MakeModel(wchar_t *pszModelName) = 0;
	virtual model *MakeModel(const std::vector<vertex>& vertices) = 0;
	virtual model *MakeModel(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) = 0;
	virtual composite* MakeModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, point_precision rotateY) = 0;

	// TODO: Fix this
	virtual composite *LoadModel(ObjectStore* pSceneGraph, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, point_precision rotateY = 0) = 0;

	virtual user *MakeUser() = 0;

	virtual composite *MakeComposite() = 0;
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