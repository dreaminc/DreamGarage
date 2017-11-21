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

#define REFTIMES_PER_MILLISEC  10000
#define REFTIMES_PER_SEC (REFTIMES_PER_MILLISEC * 100)   
#define WASAPI_WAIT_BUFFER_TIMEOUT_MS 2000

class WASAPISoundClient : public SoundClient {

public:
	WASAPISoundClient();
	~WASAPISoundClient();

	virtual RESULT Initialize() override;
	virtual RESULT AudioRenderProcess() override;
	//virtual RESULT AudioCaptureProcess() override;

private:
	RESULT InitializeAudioClient();

	RESULT EnumerateWASAPIDevices();
	RESULT EnumerateWASAPISessions();
	std::wstring GetDeviceName(IMMDeviceCollection *pDeviceCollection, UINT DeviceIndex);

	RESULT PrintWaveFormat();

private:
	// TODO: Move these to member vars
	IMMDeviceEnumerator *m_pEnumerator = nullptr;
	IMMDevice *m_pAudioEndpointDevice = nullptr;
	IAudioClient *m_pAudioClient = nullptr;
	IAudioCaptureClient *m_pCaptureClient = nullptr;
	IAudioRenderClient *m_pRenderClient = nullptr;
	IAudioSessionManager2* m_pSessionManager = nullptr;
	WAVEFORMATEX *m_pWaveFormatX = nullptr;

	REFERENCE_TIME m_hnsRequestedDuration = REFTIMES_PER_SEC;
};

#endif WASAPI_SOUND_CLIENT_H_