#ifndef XAUDIO2_SOUND_CLIENT_H_
#define XAUDIO2_SOUND_CLIENT_H_

#include "core/ehm/EHM.h"

// Dream Sound XAudio2
// dos/src/sound/win64/xaudio2/XAudio2SoundClient.h

#include <map>

#include <Windows.h>
#include <XAudio2.h>
#include <x3daudio.h>

#include "Sound/SoundClient.h"

#define REFTIMES_PER_MILLISEC  10000
#define REFTIMES_PER_SEC (REFTIMES_PER_MILLISEC * 100)   
#define WASAPI_WAIT_BUFFER_TIMEOUT_MS 2000

class point;
class vector;

class XAudio2SoundClient : 
	public SoundClient,
	public IXAudio2VoiceCallback
{

public:
	XAudio2SoundClient(std::wstring *pwstrOptAudioOutputGUID = nullptr);
	~XAudio2SoundClient();

	virtual RESULT Initialize() override;

	virtual RESULT AudioRenderProcess() override;
	virtual RESULT AudioCaptureProcess() override;
	virtual RESULT AudioSpatialProcess() override;

	// TODO: 
	virtual std::shared_ptr<SpatialSoundObject> MakeSpatialAudioObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) override;

	// XAudio doesn't need buffers apparently 
	virtual RESULT PlaySoundFile(SoundFile *pSoundFile) override;
	virtual RESULT LoopSoundFile(SoundFile *pSoundFile) override;
	virtual RESULT PushAudioPacket(const AudioPacket &pendingAudioPacket) override;
	virtual RESULT PlayAudioPacketSigned16Bit(const AudioPacket &pendingAudioPacket, std::string strAudioTrackLabel, int channel) override;

	// IXAudio2VoiceCallback
	virtual void OnStreamEnd() override { }
	virtual void OnVoiceProcessingPassEnd() override { }
	virtual void OnVoiceProcessingPassStart(UINT32 SamplesRequired) override { }
	virtual void OnBufferStart(void * pBufferContext) override { }
	virtual void OnLoopEnd(void * pBufferContext) override { }
	virtual void OnVoiceError(void * pBufferContext, HRESULT Error) override { }
	virtual void OnBufferEnd(void * pBufferContext) override;

private:
	//RESULT InitializeRenderAudioClient();
	//RESULT InitializeCaptureAudioClient();
	//RESULT InitializeSpatialAudioClient();

private:
	//RESULT PrintWaveFormat(WAVEFORMATEX *pWaveFormatX, std::string strInfo = "default");

private:
	std::shared_ptr<IXAudio2> m_pXAudio2 = nullptr;

	std::shared_ptr<IXAudio2MasteringVoice> m_pXAudio2MasterVoice = nullptr;

	std::shared_ptr<IXAudio2SourceVoice> m_pXAudio2SourceVoiceStereoFloat32 = nullptr;
	std::shared_ptr<IXAudio2SourceVoice> m_pXAudio2SourceVoiceStereoSignedInt16 = nullptr;

	int m_numMonoChannels = 8;
	std::map <int, std::shared_ptr<IXAudio2SourceVoice>> m_xaudio2MonoSignedInt16Sources;

	// Spatial
	std::shared_ptr<X3DAUDIO_HANDLE> m_pX3DInstance = nullptr;
	std::shared_ptr<X3DAUDIO_LISTENER> m_pX3DListener = nullptr;
};

#endif XAUDIO2_SOUND_CLIENT_H_