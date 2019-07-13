#include "SoundClientFactory.h"

#if defined(_WIN32) 
	#if defined(_WIN64)
		#include "sandbox/win64/Win64Sandbox.h"
	#else
		#include "sandbox/win64/Win64Sandbox.h"
	#endif
#elif defined(__APPLE__)
	#include "sandbox/osx/OSXSandbox.h"
#elif defined(__linux__)
	#include "sandbox/linux/LinuxSandbox.h"
#endif

#include "sound/win64/wasapi/WASAPISoundClient.h"
#include "sound/win64/xaudio2/XAudio2SoundClient.h"

SoundClient* SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE type, std::wstring *pwstrOptAudioOutputGUID) {
	RESULT r = R_PASS;

	SoundClient *pSoundClient = nullptr;

	switch (type) {
		case SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI: {
	#if defined(_WIN32)
			pSoundClient = new WASAPISoundClient(pwstrOptAudioOutputGUID);
	#else
			pSoundClient = nullptr;
			DEBUG_LINEOUT("Sound Client type %d not supported on this platform!", type);
	#endif
		} break;

		case SOUND_CLIENT_TYPE::SOUND_CLIENT_XAUDIO2: {
#if defined(_WIN32)
			pSoundClient = new XAudio2SoundClient(pwstrOptAudioOutputGUID);
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