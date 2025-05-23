#ifndef WASAPI_SOUND_CLIENT_H_
#define WASAPI_SOUND_CLIENT_H_

#include "core/ehm/EHM.h"

// Dream Sound
// dos/src/sound/win64/wasapi/WASAPISoundClient.h

#include <Windows.h>
#include <AudioClient.h>
#include <SpatialAudioClient.h>
#include <SpatialAudioHrtf.h>

#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <avrt.h>
#include <functiondiscoverykeys.h>

#include <wrl/client.h>

#include "sound/SoundClient.h"

#define REFTIMES_PER_MILLISEC  10000
#define REFTIMES_PER_SEC (REFTIMES_PER_MILLISEC * 100)   
#define WASAPI_WAIT_BUFFER_TIMEOUT_MS 2000

class WASAPISoundClient : public SoundClient {

public:
	WASAPISoundClient(std::wstring *pwstrOptAudioOutputGUID = nullptr);
	~WASAPISoundClient();

	virtual RESULT Initialize() override;

	virtual RESULT AudioRenderProcess() override;
	virtual RESULT AudioCaptureProcess() override;
	virtual RESULT AudioSpatialProcess() override;

	virtual std::wstring GetDeviceIDFromDeviceID(std::wstring wstrDeviceOutGUID) override;

private:
	RESULT InitializeRenderAudioClient();
	RESULT InitializeCaptureAudioClient();
	RESULT InitializeSpatialAudioClient();

	RESULT EnumerateWASAPIDevices();
	RESULT EnumerateWASAPISessions();
	std::wstring GetDeviceName(IMMDeviceCollection *pDeviceCollection, UINT DeviceIndex);
	std::wstring GetDeviceName(IMMDevice *pDevice);

	RESULT PrintWaveFormat(WAVEFORMATEX *pWaveFormatX, std::string strInfo = "default");

	virtual std::shared_ptr<SpatialSoundObject> MakeSpatialAudioObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) override;

private:
	IMMDeviceEnumerator *m_pDeviceEnumerator = nullptr;
	IAudioSessionManager2* m_pSessionManager = nullptr;

	// Spatial 
	IMMDevice *m_pAudioEndpointSpatialDevice = nullptr;
	ISpatialAudioClient *m_pAudioSpatialClient = nullptr;
	ISpatialAudioObjectRenderStreamForHrtf* m_pSpatialAudioStreamForHrtf = nullptr;
	HANDLE m_hSpatialBufferEvent = nullptr;

	// Render
	IMMDevice *m_pAudioEndpointRenderDevice = nullptr;
	IAudioClient *m_pAudioRenderClient = nullptr;
	IAudioRenderClient *m_pRenderClient = nullptr;
	WAVEFORMATEX *m_pRenderWaveFormatX = nullptr;
	REFERENCE_TIME m_hnsRequestedRenderDuration = REFTIMES_PER_SEC;

	// Capture
	IMMDevice *m_pAudioEndpointCaptureDevice = nullptr;
	IAudioClient *m_pAudioCaptureClient = nullptr;
	IAudioCaptureClient *m_pCaptureClient = nullptr;
	WAVEFORMATEX *m_pCaptureWaveFormatX = nullptr;
	REFERENCE_TIME m_hnsRequestedCaptureDuration = REFTIMES_PER_SEC;
};

#endif WASAPI_SOUND_CLIENT_H_