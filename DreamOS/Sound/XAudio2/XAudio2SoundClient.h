#ifndef XAUDIO2_SOUND_CLIENT_H_
#define XAUDIO2_SOUND_CLIENT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/XAudio2SoundClient.h

#include "Sound/SoundClient.h"

#include <Windows.h>
#include <XAudio2.h>
#include <x3daudio.h>

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
	virtual std::shared_ptr<SpatialSoundObject> MakeSpatialAudioObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) override;

	// XAudio doesn't need buffers apparently 
	virtual RESULT PlaySoundFile(SoundFile *pSoundFile) override;
	virtual RESULT PlayAudioPacket(AudioPacket pendingAudioPacket) override;

private:
	//RESULT InitializeRenderAudioClient();
	//RESULT InitializeCaptureAudioClient();
	//RESULT InitializeSpatialAudioClient();

private:

	//RESULT PrintWaveFormat(WAVEFORMATEX *pWaveFormatX, std::string strInfo = "default");

private:
	std::shared_ptr<IXAudio2> m_pXAudio2 = nullptr;

	std::shared_ptr<IXAudio2MasteringVoice> m_pXAudio2MasterVoice = nullptr;

	std::shared_ptr<IXAudio2SourceVoice> m_pXAudio2SourceVoice = nullptr;

	// Spatial
	std::shared_ptr<X3DAUDIO_HANDLE> m_pX3DInstance = nullptr;
	std::shared_ptr<X3DAUDIO_LISTENER> m_pX3DListener = nullptr;
};

#endif XAUDIO2_SOUND_CLIENT_H_