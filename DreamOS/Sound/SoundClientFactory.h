#ifndef SOUND_CLIENT_FACTORY_H_
#define SOUND_CLIENT_FACTORY_H_

// DREAM OS
// DreamOS/Sound/SoundClientFactory.h
// Allows the creation of sound clients

#include "SoundClient.h"

typedef enum {
	SOUND_CLIENT_WASAPI,
	SOUND_CLIENT_INVALID
} SOUND_CLIENT_TYPE;

class SoundClientFactory {
public:
	static SoundClient* MakeSoundClient(SOUND_CLIENT_TYPE type);
};

#endif // ! SOUND_CLIENT_FACTORY_H_