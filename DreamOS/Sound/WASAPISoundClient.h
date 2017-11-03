#ifndef WASAPI_SOUND_CLIENT_H_
#define WASAPI_SOUND_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/WASAPISoundClient.h

#include "SoundClient.h"


#include <Windows.h>
#include <AudioClient.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <avrt.h>
#include <functiondiscoverykeys.h>

class WASAPISoundClient : public SoundClient {

public:
	WASAPISoundClient();
	~WASAPISoundClient();

	virtual RESULT Initialize() override;

private:

};

#endif WASAPI_SOUND_CLIENT_H_