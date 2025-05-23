#ifndef SOUND_CLIENT_FACTORY_H_
#define SOUND_CLIENT_FACTORY_H_

#include "core/ehm/EHM.h"

// Dream Sound
// dos/src/sound/SoundClientFactory.h

// Allows the creation of sound clients

#include "SoundClient.h"

typedef enum {
	SOUND_CLIENT_WASAPI,
	SOUND_CLIENT_XAUDIO2,
	SOUND_CLIENT_INVALID
} SOUND_CLIENT_TYPE;

class SoundClientFactory {
public:
	static SoundClient* MakeSoundClient(SOUND_CLIENT_TYPE type, std::wstring *pwstrOptAudioOutputGUID = nullptr);
};

#endif // ! SOUND_CLIENT_FACTORY_H_