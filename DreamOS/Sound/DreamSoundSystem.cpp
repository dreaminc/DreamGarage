#include "DreamSoundSystem.h"

DreamSoundSystem::DreamSoundSystem(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamSoundSystem>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamSoundSystem::~DreamSoundSystem() {
	Shutdown();
}

RESULT DreamSoundSystem::InitializeModule(void *pContext) {
	RESULT r = R_PASS;

	SetModuleName("DreamSoundSystem");
	SetModuleDescription("The Dream Sound System Module");

	CR(r);

	/* TODO: This is where we can detect the audio routings to pass to the sound system
	auto pHMD = GetDOS()->GetHMD();

	if (pHMD != nullptr) {
		auto deviceType = pHMD->GetDeviceType();

		switch (deviceType) {
		
			case HMDDeviceType::OCULUS: {
				// TODO: 
			} break;
			
			case HMDDeviceType::VIVE: {
				// TODO: 
			} break;
			
			case HMDDeviceType::META: {
				// TODO: 
			} break;
		
		}
	}
	*/

Error:
	return r;
}

RESULT DreamSoundSystem::Update(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamSoundSystem::OnModuleDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamSoundSystem::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}