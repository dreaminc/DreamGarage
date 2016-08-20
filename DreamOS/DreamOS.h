#ifndef DREAM_OS_H_
#define DREAM_OS_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamOS.h
// The DreamOS base class which a DreamOS "application" is derived from
// In the current context of use, this is referring to the application layer of the DreamOS
// client rather than a DreamOS app which will later be rolled out such that applications
// can be run within a client, making a transition to treating Dream as true operating system.
// In the short term, however, DreamOS will run in the context of an application on a given platform
// such as Android or Windows. 

#include "Primitives//Types/UID.h"
#include "Primitives/valid.h"
#include "Primitives/version.h"

#include "Sandbox/SandboxFactory.h"

#define DREAM_OS_VERSION_MAJOR 0
#define DREAM_OS_VERSION_MINOR 1
#define DREAM_OS_VERSION_MINOR_MINOR 0

#include "Primitives/light.h"
#include "Primitives/quad.h"
#include "Primitives/sphere.h"
#include "Primitives/volume.h"
#include "Primitives/text.h"
#include "Primitives/texture.h"
#include "Primitives/skybox.h"

class DreamOS : public valid {
public:
	DreamOS();
	~DreamOS();

	RESULT Initialize(int argc = 0, const char *argv[] = nullptr);
	RESULT Start();
	RESULT Exit(RESULT exitcode);

	virtual RESULT LoadScene() = 0;
	virtual RESULT Update(void) = 0;

protected:
	light *AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	light *MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);

	quad *AddQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr);

	sphere *AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));
	sphere *MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));

	text *AddText(const std::wstring& fontName, const std::string& content, double size = 1.0f, bool isBillboard = false);
	
	volume *MakeVolume(double side);
	volume *AddVolume(double width, double length, double height);
	volume *MakeVolume(double width, double length, double height);
	
	texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	
	skybox *AddSkybox();
	skybox *MakeSkybox();

	model *AddModel(wchar_t *pszModelName);
	model *MakeModel(wchar_t *pszModelName);

	RESULT AddModel(const std::wstring& strRootFolder, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, point_precision rotateY = 0);

	camera* GetCamera();
	point GetCameraPosition();
	quaternion GetCameraOrientation();

	// Cloud Controller
protected:
	RESULT RegisterHeadUpdateMessageCallback(HandleHeadUpdateMessageCallback fnHandleHeadUpdateMessageCallback);
	RESULT RegisterHandUpdateMessageCallback(HandleHandUpdateMessageCallback fnHandleHandUpdateMessageCallback);

	RESULT SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT SendUpdateHandMessage(long userID, hand::HandState handState);

protected:
	RESULT RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback);
	RESULT UnregisterUpdateCallback();

private:
	SandboxApp *m_pSandbox;

private:
	version m_versionDreamOS;
	UID m_uid;
};

#endif	// ! DREAM_OS_H_
