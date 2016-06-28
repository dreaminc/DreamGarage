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
#include "Primitives/sphere.h"

class DreamOS : public valid {
public:
	DreamOS();
	~DreamOS();

	RESULT Initialize();
	RESULT Start();
	RESULT Exit(RESULT exitcode);

	virtual RESULT LoadScene() = 0;

protected:
	//RESULT AddLight(light *pLight);
	light *AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	sphere *AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions);
	volume *AddVolume(double side);

private:
	SandboxApp *m_pSandbox;

private:
	version m_versionDreamOS;
	UID m_uid;
};

#endif	// ! DREAM_OS_H_
