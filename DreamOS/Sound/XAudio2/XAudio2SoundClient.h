#ifndef XAUDIO2_SOUND_CLIENT_H_
#define XAUDIO2_SOUND_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/XAudio2SoundClient.h

#include "Sound/SoundClient.h"

#include <Windows.h>

#define REFTIMES_PER_MILLISEC  10000
#define REFTIMES_PER_SEC (REFTIMES_PER_MILLISEC * 100)   
#define WASAPI_WAIT_BUFFER_TIMEOUT_MS 2000

class point;
class vector;

class XAudio2SoundClient : public SoundClient {

public:
	XAudio2SoundClient();
	~XAudio2SoundClient();

	virtual RESULT Initialize() override;

	virtual RESULT AudioRenderProcess() override;
	virtual RESULT AudioCaptureProcess() override;
	virtual RESULT AudioSpatialProcess() override;

	// TODO: 
	virtual std::shared_ptr<SpatialSoundObject> MakeSpatialAudioObject(point ptPosition) override;

private:

	//RESULT PrintWaveFormat(WAVEFORMATEX *pWaveFormatX, std::string strInfo = "default");

private:
	// TODO:
};

#endif XAUDIO2_SOUND_CLIENT_H_