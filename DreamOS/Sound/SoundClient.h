#ifndef SOUND_CLIENT_H_
#define SOUND_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundClient.h

class SoundClient {

public:
	SoundClient();
	~SoundClient();

	virtual RESULT Initialize() = 0;

private:
	
};

#endif SOUND_CLIENT_H_