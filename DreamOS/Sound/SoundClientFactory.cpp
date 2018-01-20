#include "SoundClientFactory.h"

#if defined(_WIN32) 
// 'Add an optimus prime comment' - Idan
//extern "C" {
//	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}
	#if defined(_WIN64)
		#include "./Sandbox/Windows/Windows64App.h"
	#else
		#include "./Sandbox/Windows/Windows64App.h"
	#endif
#elif defined(__APPLE__)
	#include "./Sandbox/OSX/OSXSandboxApp.h"
#elif defined(__linux__)
	#include "./Sandbox/Linux/LinuxApp.h"
#endif

#include "WASAPISoundClient.h"

SoundClient* SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE type) {
	RESULT r = R_PASS;

	SoundClient *pSoundClient = nullptr;

	switch (type) {
		case SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI: {
	#if defined(_WIN32)
			pSoundClient = new WASAPISoundClient();
	#else
			pSoundClient = nullptr;
			DEBUG_LINEOUT("Sound Client type %d not supported on this platform!", type);
	#endif
		} break;
	}

	// Set up the Sandbox
	CRM(pSoundClient->Initialize(), "Failed to initialize sound client");

	return pSoundClient;
Error:
	if (pSoundClient != nullptr) {
		delete pSoundClient;
		pSoundClient = nullptr;
	}

	return nullptr;
}