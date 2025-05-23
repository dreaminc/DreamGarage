﻿#include "WASAPISoundClient.h"

#include <string>

// TODO: This should get moved up to SoundClient
#include "WASAPISpatialSoundObject.h"

#include "Core/Utilities.h"

// File specific utilities
template <class T> RESULT SafeRelease(T **ppT) {
	RESULT r = R_PASS;

	CN(*ppT);

	(*ppT)->Release();
	*ppT = nullptr;

Error:
	return r;
}

WASAPISoundClient::WASAPISoundClient(std::wstring *pwstrOptAudioOutputGUID) :
	SoundClient(pwstrOptAudioOutputGUID)
{
	// empty
}

WASAPISoundClient::~WASAPISoundClient() {
	CoTaskMemFree(m_pRenderWaveFormatX);

	SafeRelease<IMMDeviceEnumerator>(&m_pDeviceEnumerator);
	SafeRelease<IMMDevice>(&m_pAudioEndpointRenderDevice);
	SafeRelease<IAudioClient>(&m_pAudioRenderClient);
	
	// TODO: Release spatial shits
}

RESULT WASAPISoundClient::EnumerateWASAPISessions() {
	RESULT r = R_PASS;
	HRESULT hr = S_OK;

	int cbSessionCount = 0;

	LPWSTR pswSessionDisplayName = nullptr;
	LPWSTR pswSessionIconPath = nullptr;
	LPWSTR pswSessionIdentifier = nullptr;

	IAudioSessionEnumerator* pSessionList = nullptr;
	IAudioSessionControl* pSessionControl = nullptr;
	IAudioSessionControl2* pSessionControl2 = nullptr;

	// Sessions
	CRM((RESULT)m_pAudioEndpointRenderDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&m_pSessionManager), "Failed to activate session manager 2 interface");
	CN(m_pSessionManager);

	// Get the current list of sessions.
	CR((RESULT)m_pSessionManager->GetSessionEnumerator(&pSessionList));
	CN(pSessionList);

	// Get the session count.
	CR((RESULT)pSessionList->GetCount(&cbSessionCount));

	DEBUG_LINEOUT("Sessions for device");

	for (int index = 0; index < cbSessionCount; index++) {
		
		CoTaskMemFree(pswSessionDisplayName);
		
		//SAFE_RELEASE(pSessionControl);

		if (pSessionControl != nullptr) {
			pSessionControl = nullptr;
		}

		// Get the <n>th session.
		CR((RESULT)pSessionList->GetSession(index, &pSessionControl));

		// Get the extended session control interface pointer.
		CR((RESULT)pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2));
		CN(pSessionControl2);

		CR((RESULT)pSessionControl2->GetSessionIdentifier(&pswSessionIdentifier));

		CR((RESULT)pSessionControl->GetDisplayName(&pswSessionDisplayName));
		CR((RESULT)pSessionControl->GetIconPath(&pswSessionIconPath));

		DEBUG_LINEOUT("Session ID: %S Name: %S path: %S", pswSessionIdentifier, pswSessionDisplayName, pswSessionIconPath);
	}

Error:
	return r;
}

std::wstring WASAPISoundClient::GetDeviceIDFromDeviceID(std::wstring wstrDeviceOutID) {
	RESULT r = R_PASS;

	LPWSTR pwszDeviceID;
	std::wstring wstrResult = L"default";
	
	UINT deviceCount = 0;
	int cbSessionCount = 0;
	bool fFound = false;

	IMMDeviceCollection *pDeviceCollection = nullptr;
	IMMDevice *pDevice = nullptr;

	CR((RESULT)m_pDeviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &pDeviceCollection));
	CR((RESULT)pDeviceCollection->GetCount(&deviceCount));

	for (UINT i = 0; i < deviceCount; i++) {

		CR((RESULT)pDeviceCollection->Item(i, &pDevice));
		CN(pDevice);

		CR((RESULT)pDevice->GetId(&pwszDeviceID));
		std::wstring wstrDeviceID = std::wstring(pwszDeviceID);

		if(wstrDeviceID == wstrDeviceOutID) {

			IPropertyStore *pPropertyStore = nullptr;
			CR((RESULT)pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore));

			DWORD numProps;
			pPropertyStore->GetCount(&numProps);

			for (int j = 0; j < (int)numProps; j++) {

				// TODO: the 45 is also flimsy
				PROPERTYKEY propKeyTemp;
				CR((RESULT)pPropertyStore->GetAt((DWORD)(j), &propKeyTemp));

				PROPVARIANT propVariant;
				PropVariantInit(&propVariant);

				CR((RESULT)pPropertyStore->GetValue(propKeyTemp, &propVariant));

				if(propVariant.vt == 31) {
					CNM(propVariant.pwszVal, "Device GUID info is nullptr");
					std::wstring wstrTemp = std::wstring(propVariant.pwszVal);

					if (wstrTemp.find(L"MMDEVAPI") != std::string::npos) {
						SafeRelease(&pPropertyStore);
						PropVariantClear(&propVariant);
						CoTaskMemFree(pwszDeviceID);

						return wstrTemp;
					}
				}
			}
		}
	}

Error:
	return wstrResult;
}

std::wstring WASAPISoundClient::GetDeviceName(IMMDevice *pDevice) {
	RESULT r = R_PASS;

	LPWSTR deviceId;
	std::wstring wstrResult = L"";

	CR((RESULT)pDevice->GetId(&deviceId));

	IPropertyStore *pPropertyStore;
	CR((RESULT)pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore));

	PROPVARIANT propVariantFriendlyName;
	PropVariantInit(&propVariantFriendlyName);

	CR((RESULT)pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propVariantFriendlyName));

	/* Moar infoz
	DWORD numProps;
	pPropertyStore->GetCount(&numProps);
	
	for (int i = 0; i < (int)numProps; i++) {
		PROPERTYKEY propKeyTemp;
	
		CR((RESULT)pPropertyStore->GetAt((DWORD)(i), &propKeyTemp));
	
		PROPVARIANT propVariantTemp;
		PropVariantInit(&propVariantTemp);
	
		CR((RESULT)pPropertyStore->GetValue(propKeyTemp, &propVariantTemp));
	
		if(propVariantTemp.vt == 31 && propVariantTemp.pwszVal != nullptr) {
			//if (propKeyTemp == PKEY_AudioEndpoint_GUID) {
			//if (i == 55) {
			//	int a = 5;
			//}

			DEBUG_LINEOUT("%d: %S", i, propVariantTemp.pwszVal);
		}
	}
	//*/

	SafeRelease(&pPropertyStore);

	wstrResult = std::wstring(propVariantFriendlyName.pwszVal);

	PropVariantClear(&propVariantFriendlyName);
	CoTaskMemFree(deviceId);

Error:
	return wstrResult;
}

std::wstring WASAPISoundClient::GetDeviceName(IMMDeviceCollection *pDeviceCollection, UINT DeviceIndex) {
	RESULT r = R_PASS;

	IMMDevice *pDevice = nullptr;
	std::wstring wstrResult = L"";

	CR((RESULT)pDeviceCollection->Item(DeviceIndex, &pDevice));
	CN(pDevice);

	wstrResult = GetDeviceName(pDevice);

Error:
	if (pDevice != nullptr) {
		SafeRelease(&pDevice);
		pDevice = nullptr;
	}

	return wstrResult;
}

// TODO: This simply enumerates the devices but doesn't select one
RESULT WASAPISoundClient::EnumerateWASAPIDevices() {
	RESULT r = R_PASS;
	HRESULT hr = S_OK;

	// TODO: Do we want to keep the collection as member?
	IMMDeviceCollection *pDeviceCollection = nullptr;
	IMMDevice *pDevice = nullptr;

	// TODO: Put into WindowsApp ? 
	CRM((RESULT)CoInitializeEx(NULL, COINIT_MULTITHREADED), "Faield to initialize COM library");

	CRM((RESULT)CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
		(void**)&m_pDeviceEnumerator),
		"CoCreateInstance Failed");
	CN(m_pDeviceEnumerator);

	CR((RESULT)m_pDeviceEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &pDeviceCollection));

	UINT deviceCount;
	CR((RESULT)pDeviceCollection->GetCount(&deviceCount));

	for (UINT i = 0; i < deviceCount; i++) {
		//String deviceName = GetDeviceName(pDeviceCollection, DeviceIndex);
		//Console::WriteLine(String(DeviceIndex) + String(": ") + deviceName);
		std::wstring wstrDeviceName = GetDeviceName(pDeviceCollection, i);
		DEBUG_LINEOUT("Device: %S", wstrDeviceName.c_str());
	}

	/*
	int deviceIndex = 0;
	if (AudioDeviceIndex < deviceCount) {
		deviceIndex = AudioDeviceIndex;
	}

	hr = pDeviceCollection->Item(deviceIndex, DeviceToUse);
	PersistentAssert(DeviceToUse != NULL && SUCCEEDED(hr), "deviceCollection->Item failed");
	*/

	SafeRelease(&pDeviceCollection);

Error:
	return r;
}

const char* GetAudioClientErrorCodeString(HRESULT hr) {
	switch (hr) {

	case AUDCLNT_E_BUFFER_ERROR: {
		return "GetBuffer failed to retrieve a data buffer and *ppData points to NULL";
	} break;

	case AUDCLNT_E_BUFFER_TOO_LARGE: {
		return "The NumFramesRequested value exceeds the available buffer space(buffer size minus padding size).";
	} break;

	case AUDCLNT_E_BUFFER_SIZE_ERROR: {
		return "The stream is exclusive mode and uses event - driven buffering, but the client attempted to get a packet that was not the size of the buffer.";
	} break;

	case AUDCLNT_E_OUT_OF_ORDER: {
		return "A previous IAudioRenderClient::GetBuffer call is still in effect.";
	} break;
			
	case AUDCLNT_E_DEVICE_INVALIDATED: {
		return "The audio endpoint device has been unplugged, or the audio hardware or associated hardware resources have been reconfigured, disabled, removed, or otherwise made unavailable for use.";
	} break;
			
	case AUDCLNT_E_BUFFER_OPERATION_PENDING: {
		return "Buffer cannot be accessed because a stream reset is in progress.";
	} break;
			
	case AUDCLNT_E_SERVICE_NOT_RUNNING: {
		return "The Windows audio service is not running.";
	} break;

	case E_POINTER: {
		return "Parameter ppData is NULL.";
	} break;

	}

	return "Non-handled HR value";
}

RESULT WASAPISoundClient::AudioCaptureProcess() {
	RESULT r = R_PASS;
	HRESULT hr = S_OK;

	// TODO: Move to member?
	IAudioCaptureClient *pCaptureClient = nullptr;

	HANDLE hCaptureBufferEvent = nullptr;
	HANDLE hAudioCaptureProcessTask = nullptr;

	UINT32 bufferFrameCount = 0;
	DWORD audioDeviceFlags = 0;
	BYTE *pAudioCaptureBufferData = nullptr;
	UINT32 packetLength = 0;
	UINT32 numFramesAvailable;

	DEBUG_LINEOUT("WASAPISoundClient::AudioCaptureProcess Start");

	CNM(m_pAudioCaptureClient, "Audio Capture Client not initialized");

	// Create an event handle and register it for
	// buffer-event notifications.
	hCaptureBufferEvent = CreateEvent(nullptr, false, false, nullptr);
	CNM(hCaptureBufferEvent, "Failed to create capture event");

	CRM((RESULT)m_pAudioCaptureClient->SetEventHandle(hCaptureBufferEvent), "Failed to register capture event with audio client");

	// Get the actual size of the two allocated buffers.
	CRM((RESULT)m_pAudioCaptureClient->GetBufferSize(&bufferFrameCount), "Failed to retrieve buffer size");
	CB((bufferFrameCount != 0));

	// Capture Client
	CRM((RESULT)m_pAudioCaptureClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient), "Failed to get capture audio client");
	CN(pCaptureClient);

	// Ask MMCSS to temporarily boost the thread priority
	// to reduce glitches while the low-latency stream plays.
	DWORD taskIndex = 0;
	hAudioCaptureProcessTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	CNM(hAudioCaptureProcessTask, "Failed to set up audio capture process task");

	CRM((RESULT)m_pAudioCaptureClient->Start(), "Failed to start capture audio device");

	// Each loop fills one of the two buffers.
	while (audioDeviceFlags != AUDCLNT_BUFFERFLAGS_SILENT && m_captureState != sound::state::STOPPED) {

		// Wait for next buffer event to be signaled.
		DWORD retval = WaitForSingleObject(hCaptureBufferEvent, WASAPI_WAIT_BUFFER_TIMEOUT_MS);

		// Event handle timed out after the default time out
		if (retval != WAIT_OBJECT_0) {
			m_pAudioCaptureClient->Stop();
			CRM((RESULT)(ERROR_TIMEOUT), "Capture audio thread is hung and exited");
		}

		// Check Capture
		hr = pCaptureClient->GetNextPacketSize(&packetLength);
		CR((RESULT)hr);

		if (packetLength > 0) {
			//DEBUG_LINEOUT("capture %d", packetLength);

			while (packetLength != 0) {
				// Get the available data in the shared buffer.
				hr = pCaptureClient->GetBuffer(&pAudioCaptureBufferData, &numFramesAvailable, &audioDeviceFlags, nullptr, nullptr);
				CR((RESULT)hr);

				// TODO: We might want to handle different kind of 
				// format types here - or convert elsewhere 

				// Set to float data buffer
				float *pFloatDataBuffer = (float*)(pAudioCaptureBufferData);

				// Check for silence
				if (audioDeviceFlags & AUDCLNT_BUFFERFLAGS_SILENT) {
					pAudioCaptureBufferData = nullptr;  
				}

				int16_t *pSigned16IntBuffer = nullptr;
				pSigned16IntBuffer = new int16_t[numFramesAvailable];
				CN(pSigned16IntBuffer);

				// Convert to signed int data buffer
				double avgAccumulator = 0.0f;
				
				for (unsigned int i = 0; i < numFramesAvailable; i++) {
					pSigned16IntBuffer[i] = pFloatDataBuffer[i] * std::numeric_limits<int16_t>::max();
					avgAccumulator += std::abs(pFloatDataBuffer[i]);
				}

				// TODO: This is not actually the run time average
				m_runTimeCaptureAverage = (avgAccumulator / numFramesAvailable);

				m_pCaptureSoundBuffer->LockBuffer();
				{
					if (m_pCaptureSoundBuffer->IsFull() == false) {
						CR(m_pCaptureSoundBuffer->PushData(pSigned16IntBuffer, numFramesAvailable, m_pCaptureWaveFormatX->nSamplesPerSec));

						// This will trigger the observer 
						CR(HandleAudioDataCaptured(numFramesAvailable));
					}
					else {
						DEBUG_LINEOUT("Capture buffer is full");
					}
				}
				m_pCaptureSoundBuffer->UnlockBuffer();

				// De-alloc the temp thing
				if (pSigned16IntBuffer != nullptr) {
					delete[] pSigned16IntBuffer;
					pSigned16IntBuffer = nullptr;
				}

				hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
				CR((RESULT)hr);

				hr = pCaptureClient->GetNextPacketSize(&packetLength);
				CR((RESULT)hr);
			}

		}
	}

	// Wait for the last buffer to play before stopping.
	Sleep((DWORD)(m_hnsRequestedCaptureDuration / REFTIMES_PER_MILLISEC));

	CRM((RESULT)m_pAudioCaptureClient->Stop(), "Failed to stop audio capture client");

Error:
	DEBUG_LINEOUT("WASAPISoundClient::AudioCaptureProcess Finish");

	if (hCaptureBufferEvent != nullptr) {
		CloseHandle(hCaptureBufferEvent);
		hCaptureBufferEvent = nullptr;
	}

	if (hAudioCaptureProcessTask != nullptr) {
		AvRevertMmThreadCharacteristics(hAudioCaptureProcessTask);
		hAudioCaptureProcessTask = nullptr;
	}

	if (pCaptureClient != nullptr) {
		SafeRelease<IAudioCaptureClient>(&pCaptureClient);
	}

	return r;
}

std::shared_ptr<SpatialSoundObject> WASAPISoundClient::MakeSpatialAudioObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) {
	RESULT r = R_PASS;

	std::shared_ptr<SpatialSoundObject> pSpatialSoundObject = nullptr;

	CNM(m_pAudioSpatialClient, "Audio Spatial Client not initialized");
	CNM(m_pSpatialAudioStreamForHrtf, "Audio Spatial HRTF not initialized");

	pSpatialSoundObject =
		std::make_shared<WASAPISpatialSoundObject>(GetSpaitalSamplingRate(), ptPosition, vEmitterDirection, vListenerDirection, m_pAudioSpatialClient, m_pSpatialAudioStreamForHrtf);
	CNM(pSpatialSoundObject, "Failed to allocate wasapi spatial sound object");

	CRM(pSpatialSoundObject->Initialize(), "Failed to initialize WASAPI HRTF spatial object");

	return pSpatialSoundObject;

Error:
	if (pSpatialSoundObject != nullptr) {
		pSpatialSoundObject = nullptr;
	}

	return nullptr;
}

RESULT WASAPISoundClient::AudioSpatialProcess() {
	RESULT r = R_PASS;

	DWORD audioDeviceFlags = 0;

	DEBUG_LINEOUT("WASAPISoundClient::AudioSpatialProcess Start");

	CNM(m_pAudioSpatialClient, "Audio Spatial Client not initialized");
	CNM(m_pSpatialAudioStreamForHrtf, "Audio Spatial HRTF not initialized");

	CRM((RESULT)m_pSpatialAudioStreamForHrtf->Start(), "Failed to start spatial audio HRTF stream");

	do {

		// Wait for next buffer event to be signaled.
		DWORD retval = WaitForSingleObject(m_hSpatialBufferEvent, WASAPI_WAIT_BUFFER_TIMEOUT_MS);

		// Event handle timed out after the default time out
		if (retval != WAIT_OBJECT_0) {
			CRM((RESULT)(ERROR_TIMEOUT), "Spatial audio thread is hung and exited");
		}

		UINT32 availableDynamicObjectCount;
		UINT32 frameCount;

		// Begin the process of sending object data and metadata
		// Get the number of active objects that can be used to send object-data
		// Get the frame count that each buffer will be filled with 
		CRM((RESULT)m_pSpatialAudioStreamForHrtf->BeginUpdatingAudioObjects(&availableDynamicObjectCount, &frameCount),
			"Failed to begin updating audio objects");

		for (auto &pSpatialSoundObject : m_spatialSoundObjects) {
			
			CR(pSpatialSoundObject->Update(frameCount, 1));

			//// Implied 48K sampling, 440 hz freq
			//pSpatialSoundObject->WriteTestSignalToAudioObjectBuffer(frameCount);	

		}
			
		// Let the audio-engine know that the object data are available for processing now
		CRM((RESULT)m_pSpatialAudioStreamForHrtf->EndUpdatingAudioObjects(), "Failed to EndUpdatingAudioObjects");

	} while (audioDeviceFlags != AUDCLNT_BUFFERFLAGS_SILENT);

Error:
	DEBUG_LINEOUT("WASAPISoundClient::AudioSpatialProcess Finish");

	if (m_hSpatialBufferEvent != nullptr) {
		CloseHandle(m_hSpatialBufferEvent);
		m_hSpatialBufferEvent = nullptr;
	}

	return r;
}

RESULT WASAPISoundClient::AudioRenderProcess() {
	RESULT r = R_PASS;

	// TODO: Move to member?
	IAudioRenderClient *pRenderClient = nullptr;

	HANDLE hRenderBufferEvent = nullptr;
	HANDLE hAudioRenderProcessTask = nullptr;
	UINT32 bufferFrameCount = 0;
	DWORD audioDeviceFlags = 0;
	BYTE *pAudioClientBufferData;
	HRESULT hr = S_OK;

	UINT32 numFramesAvailable;
	UINT32 numFramesPadding;
	UINT32 packetLength = 0;

	DEBUG_LINEOUT("WASAPISoundClient::AudioRenderProcess Start");

	CNM(m_pAudioRenderClient, "Audio Render Client not initialized");

	// Create an event handle and register it for
	// buffer-event notifications.
	hRenderBufferEvent = CreateEvent(nullptr, false, false, nullptr);
	CNM(hRenderBufferEvent, "Failed to create event");

	CRM((RESULT)m_pAudioRenderClient->SetEventHandle(hRenderBufferEvent), "Failed to register event with audio client");

	// Get the actual size of the two allocated buffers.
	CRM((RESULT)m_pAudioRenderClient->GetBufferSize(&bufferFrameCount), "Failed to retrieve buffer size");
	CB((bufferFrameCount != 0));

	// Render Client
	CRM((RESULT)m_pAudioRenderClient->GetService(__uuidof(IAudioRenderClient), (void**)&pRenderClient), "Failed to get render audio client");
	CN(pRenderClient);

	// Ask MMCSS to temporarily boost the thread priority
	// to reduce glitches while the low-latency stream plays.
	DWORD taskIndex = 0;
	hAudioRenderProcessTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);
	CNM(hAudioRenderProcessTask, "Failed to set up audio process task");

	CRM((RESULT)m_pAudioRenderClient->Start(), "Failed to start audio device");

	// Each loop fills one of the two buffers.
	while (audioDeviceFlags != AUDCLNT_BUFFERFLAGS_SILENT) {

		// Wait for next buffer event to be signaled.
		DWORD retval = WaitForSingleObject(hRenderBufferEvent, WASAPI_WAIT_BUFFER_TIMEOUT_MS);

		// Event handle timed out after the default time out
		if (retval != WAIT_OBJECT_0) {
			m_pAudioRenderClient->Stop();
			CRM((RESULT)(ERROR_TIMEOUT), "Render audio thread is hung and exited");
		}

		// See how much buffer space is available.
		hr = m_pAudioRenderClient->GetCurrentPadding(&numFramesPadding);
		CR((RESULT)hr);

		numFramesAvailable = bufferFrameCount - numFramesPadding;

		if (numFramesAvailable > 0) {
			//DEBUG_LINEOUT("render %d", numFramesAvailable);

			// Grab the next empty buffer from the audio device.
			hr = pRenderClient->GetBuffer(numFramesAvailable, &pAudioClientBufferData);
			CRM((RESULT)hr, "Failed to get buffer: %s", GetAudioClientErrorCodeString(hr));

			/*
			// TEST: Fake audio output

			static double theta = 0.0f;
			static double freq = 440.0f;

			float *pDataBuffer = (float*)(pAudioClientBufferData);

			for (uint16_t i = 0; i < (numFramesAvailable * 2); i += 2) {
				float val = sin(theta);
				val *= 0.25f;

				pDataBuffer[i] = val;
				pDataBuffer[i + 1] = val;

				// increment theta
				theta += ((2.0f * M_PI) / 44100.0f) * freq;
				if (theta >= 2.0f * M_PI) {
					theta = theta - (2.0f * M_PI);
				}
			}
			//*/

			// If there are pending buffer bytes in the render buffer
			// play it back
			int readBytes = 0;
			m_pRenderSoundBuffer->LockBuffer(); 
			{
				if ((readBytes = (int)m_pRenderSoundBuffer->NumPendingFrames()) > 0) {
					//DEBUG_LINEOUT("Rendering %d frames", readBytes);

					float *pDataBuffer = (float*)(pAudioClientBufferData);

					// Get the bytes (This will interlace them)
					CRM(m_pRenderSoundBuffer->LoadDataToInterlacedTargetBuffer(pDataBuffer, numFramesAvailable), "Failed to load render sound buffer");
				}
			}
			m_pRenderSoundBuffer->UnlockBuffer();

			hr = pRenderClient->ReleaseBuffer(numFramesAvailable, audioDeviceFlags);
			CRM((RESULT)hr, "Failed to release buffer: %s", GetAudioClientErrorCodeString(hr));
		}
	}

	// Wait for the last buffer to play before stopping.
	Sleep((DWORD)(m_hnsRequestedRenderDuration / REFTIMES_PER_MILLISEC));

	CRM((RESULT)m_pAudioRenderClient->Stop(), "Failed to stop audio render client"); 

		
Error:

	DEBUG_LINEOUT("WASAPISoundClient::AudioRenderProcess Finish");

	if (hRenderBufferEvent != nullptr) {
		CloseHandle(hRenderBufferEvent);
		hRenderBufferEvent = nullptr;
	}

	if (hAudioRenderProcessTask != nullptr) {
		AvRevertMmThreadCharacteristics(hAudioRenderProcessTask);
		hAudioRenderProcessTask = nullptr;
	}
	
	SafeRelease<IAudioRenderClient>(&pRenderClient);

	// member vars are released in destructor 

	return r;
}

const char *GetFormatTagString(DWORD wFormatTag) {
	switch (wFormatTag) {
		case WAVE_FORMAT_PCM: return "WAVE_PCM"; break;
		case WAVE_FORMAT_EXTENSIBLE: return "WAVE_FORMAT_EXTENSIBLE"; break;
	}

	return "UNDEFINED FORMAT";
}

const char *GetSubFormatString(GUID waveFormatSubFormat) {
	if(waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_ADPCM)
		return "Adaptive delta pulse code modulation(ADPCM)";			
	else if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_ALAW)
		return "A - law coding.";
	else if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_DRM)
		return "DRM - encoded format for digital - audio content protected by Microsoft Digital Rights Management.";
	else if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_IEC61937_DOLBY_DIGITAL_PLUS)
		return "Dolby Digital Plus formatted for HDMI output.";
	else if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_IEC61937_DOLBY_DIGITAL)
		return "Dolby Digital Plus formatted for S / PDIF or HDMI output.";
	else if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
		return "IEEE floating - point audio.";
	else if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_MPEG)
		return "MPEG - 1 audio payload.";
	else if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_MULAW)
		return "micro - law coding";
	else if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_PCM)
		return "PCM audio";

	return "UNDEFINED SUBFORMAT";
}

sound::type GetFormatSoundBufferType(WAVEFORMATEX *pWaveFormatX) {
	RESULT r = R_PASS;

	if (pWaveFormatX->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		WAVEFORMATEXTENSIBLE *pWaveFormatExtensible = (WAVEFORMATEXTENSIBLE*)pWaveFormatX;
		CN(pWaveFormatExtensible);
		
		GUID waveFormatSubFormat = pWaveFormatExtensible->SubFormat;
		
		if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
			if (pWaveFormatX->wBitsPerSample == 32)
				return sound::type::FLOATING_POINT_32_BIT;
			else if(pWaveFormatX->wBitsPerSample == 64)
				return sound::type::FLOATING_POINT_64_BIT;
		}
		else if (waveFormatSubFormat == KSDATAFORMAT_SUBTYPE_PCM) {
			if (pWaveFormatX->wBitsPerSample == 8)
				return sound::type::UNSIGNED_8_BIT;
			else if (pWaveFormatX->wBitsPerSample == 16)
				return sound::type::SIGNED_16_BIT;
		}
	}

Error:
	return sound::type::INVALID;
}

RESULT WASAPISoundClient::PrintWaveFormat(WAVEFORMATEX *pWaveFormatX, std::string strInfo) {
	RESULT r = R_PASS;

	int channels;
	int bitsPerSample;
	int samplingRate;

	CN(pWaveFormatX);

	channels = pWaveFormatX->nChannels;
	bitsPerSample = pWaveFormatX->wBitsPerSample;
	samplingRate = pWaveFormatX->nSamplesPerSec;

	DEBUG_LINEOUT("%s: Audio Format %s, %d channels, %d bits per sample, %d sampling rate", strInfo.c_str(),
		GetFormatTagString(pWaveFormatX->wFormatTag), channels, bitsPerSample, samplingRate);

	if (pWaveFormatX->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
		WAVEFORMATEXTENSIBLE *pWaveFormatExtensible = (WAVEFORMATEXTENSIBLE*)pWaveFormatX;
		CN(pWaveFormatExtensible);

		GUID waveFormatSubFormat = pWaveFormatExtensible->SubFormat;
		DEBUG_LINEOUT("Audio sub format: %s", GetSubFormatString(waveFormatSubFormat));
	}

Error:
	return r;
}

RESULT WASAPISoundClient::InitializeSpatialAudioClient() {
	RESULT r = R_PASS;

	// Default Render Endpoint
	CRM((RESULT)m_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pAudioEndpointSpatialDevice),
		"Failed to get default audio spatial endpoint");
	CN(m_pAudioEndpointSpatialDevice);
	
	{
		std::wstring wstrDeviceName = GetDeviceName(m_pAudioEndpointSpatialDevice);
		DEBUG_LINEOUT("Spatial Device: %S", wstrDeviceName.c_str());
	}
	
	CRM((RESULT)m_pAudioEndpointSpatialDevice->Activate(__uuidof(ISpatialAudioClient), CLSCTX_ALL, nullptr, (void**)&m_pAudioSpatialClient),
		"Failed to activate spatial audio client");
	CN(m_pAudioSpatialClient);
	

	// Move this to member etc
	// Spatial audio is more restrictive (mono)
	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	format.wBitsPerSample = 32;
	format.nChannels = 1;
	format.nSamplesPerSec = 48000;
	format.nBlockAlign = (format.wBitsPerSample >> 3) * format.nChannels;
	format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;
	format.cbSize = 0;

	CRM((RESULT)(m_pAudioSpatialClient->IsAudioObjectFormatSupported(&format)), "Audio format not supported");
	
	// This will fail if windows sonic or whatever isn't set up
	// Use HRTF if available

	if ((r = (RESULT)m_pAudioSpatialClient->IsSpatialAudioStreamAvailable(__uuidof(m_pSpatialAudioStreamForHrtf), NULL)) < 0) {
		m_pSpatialAudioStreamForHrtf = nullptr;
		m_fHRTFEnabled = false;
		DEBUG_LINEOUT("Spatial audio stream not available");
		CBM((false), "WASAPI Spatial not supported");
	}

	// Create the event that will be used to signal the client for more data
	m_hSpatialBufferEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	CN(m_hSpatialBufferEvent);

	UINT32 maxDynamicObjectCount;
	CRM((RESULT)m_pAudioSpatialClient->GetMaxDynamicObjectCount(&maxDynamicObjectCount), "Failed to get max dynamic object count");
	CBM((maxDynamicObjectCount > 0), "Spatial audio doesn't support any dynamic objects");
	m_maxSpatialSoundObjects = maxDynamicObjectCount;

	// TODO: Move all of this into user etc 
	SpatialAudioHrtfActivationParams streamParams;
	streamParams.ObjectFormat = &format;
	streamParams.StaticObjectTypeMask = AudioObjectType_None;
	streamParams.MinDynamicObjectCount = 0;
	streamParams.MaxDynamicObjectCount = maxDynamicObjectCount;
	streamParams.Category = AudioCategory_Communications;
	streamParams.EventHandle = m_hSpatialBufferEvent;
	streamParams.NotifyObject = NULL;

	SpatialAudioHrtfDistanceDecay decayModel;
	decayModel.CutoffDistance = 100.0f;
	decayModel.MaxGain = 3.98f;
	decayModel.MinGain = float(1.58439f * pow(10.0f, -5.0f));
	decayModel.Type = SpatialAudioHrtfDistanceDecayType::SpatialAudioHrtfDistanceDecay_NaturalDecay;
	decayModel.UnityGainDistance = 1.0f;

	streamParams.DistanceDecay = &decayModel;

	SpatialAudioHrtfDirectivity directivity;
	//directivity.Type = SpatialAudioHrtfDirectivityType::SpatialAudioHrtfDirectivity_Cone;
	directivity.Type = SpatialAudioHrtfDirectivityType::SpatialAudioHrtfDirectivity_Cardioid;
	//directivity.Type = SpatialAudioHrtfDirectivityType::SpatialAudioHrtfDirectivity_OmniDirectional;
	directivity.Scaling = 1.0f;

	SpatialAudioHrtfDirectivityCone cone;
	cone.directivity = directivity;
	cone.InnerAngle = 0.1f;
	cone.OuterAngle = 0.2f;

	SpatialAudioHrtfDirectivityCardioid cardioid; 
	cardioid.directivity = directivity;
	cardioid.Order = 2;
	
	SpatialAudioHrtfDirectivityUnion directivityUnion;
	//directivityUnion.Omni = directivity;
	//directivityUnion.Cone = cone;
	directivityUnion.Cardiod = cardioid;
	streamParams.Directivity = &directivityUnion;

	//SpatialAudioHrtfEnvironmentType environment = SpatialAudioHrtfEnvironmentType::SpatialAudioHrtfEnvironment_Small;
	SpatialAudioHrtfEnvironmentType environment = SpatialAudioHrtfEnvironmentType::SpatialAudioHrtfEnvironment_Outdoors;
	streamParams.Environment = &environment;

	// identity matrix
	SpatialAudioHrtfOrientation hrtfOrientation = { 1.0f, 0.0f, 0.0f, 
													0.0f, 1.0f, 0.0f, 
													0.0f, 0.0f, 1.0f }; 
	streamParams.Orientation = &hrtfOrientation;

	PROPVARIANT pv;
	PropVariantInit(&pv);
	pv.vt = VT_BLOB;
	pv.blob.cbSize = sizeof(streamParams);
	pv.blob.pBlobData = (BYTE*)&streamParams;

	CRM((RESULT)m_pAudioSpatialClient->ActivateSpatialAudioStream(&pv, __uuidof(m_pSpatialAudioStreamForHrtf), (void**)&m_pSpatialAudioStreamForHrtf),
			"Failed to activate spatial audio stream");

Error:
	return r;
}

RESULT WASAPISoundClient::InitializeRenderAudioClient() {
	RESULT r = R_PASS;

	//REFERENCE_TIME hnsActualDuration;

	m_hnsRequestedRenderDuration = REFTIMES_PER_SEC;

	// Default Render Endpoint
	CRM((RESULT)m_pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pAudioEndpointRenderDevice), "Failed to get default audio render endpoint");
	CN(m_pAudioEndpointRenderDevice);

	// Audio Render Client Device
	CRM((RESULT)m_pAudioEndpointRenderDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_pAudioRenderClient), "Failed to activate audio render client");
	CN(m_pAudioRenderClient);

	// Render Format 
	CR((RESULT)m_pAudioRenderClient->GetMixFormat(&m_pRenderWaveFormatX));
	CN(m_pRenderWaveFormatX);

	sound::type bufferType = GetFormatSoundBufferType(m_pRenderWaveFormatX);
	int numChannels = m_pRenderWaveFormatX->nChannels;

	// Print out format
	CR(PrintWaveFormat(m_pRenderWaveFormatX, "render"));

	//// Initialize the stream to play at the minimum latency.
	//CR((RESULT)m_pAudioClient->GetDevicePeriod(NULL, &m_hnsRequestedDuration));
	//CR((RESULT)m_pAudioClient->GetDevicePeriod(&m_hnsRequestedDuration, nullptr));

	// Initialize Render Audio Client
	CRM((RESULT)m_pAudioRenderClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED, 
		AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
		m_hnsRequestedRenderDuration, 
		0,
		m_pRenderWaveFormatX, 
		nullptr), 
	"Failed to init render client");

	// Set up the render Sound buffer
	CR(InitializeRenderSoundBuffer(numChannels, m_pRenderWaveFormatX->nSamplesPerSec, bufferType));

Error:
	return r;
}

RESULT WASAPISoundClient::InitializeCaptureAudioClient() {
	RESULT r = R_PASS;

	m_hnsRequestedCaptureDuration = REFTIMES_PER_SEC;

	// Default Capture Endpoint
	CRM((RESULT)m_pDeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &m_pAudioEndpointCaptureDevice), "Failed to get default audio capture endpoint");
	CN(m_pAudioEndpointCaptureDevice);

	// Audio Capture Client Device
	CRM((RESULT)m_pAudioEndpointCaptureDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_pAudioCaptureClient), "Failed to activate audio capture client");
	CN(m_pAudioCaptureClient);

	// Capture Format 
	CR((RESULT)m_pAudioCaptureClient->GetMixFormat(&m_pCaptureWaveFormatX));
	CN(m_pCaptureWaveFormatX);

	sound::type captureBufferType = GetFormatSoundBufferType(m_pCaptureWaveFormatX);
	int numChannels = m_pCaptureWaveFormatX->nChannels;
	int samplingRate = m_pCaptureWaveFormatX->nSamplesPerSec;

	// We want to use 16 bit signed integer for the sound buffer
	sound::type bufferType = sound::type::SIGNED_16_BIT;

	// Print out format
	CR(PrintWaveFormat(m_pCaptureWaveFormatX, "capture"));

	// Initialize Capture Audio Client
	CRM((RESULT)m_pAudioCaptureClient->Initialize(
		AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
		m_hnsRequestedCaptureDuration,
		0,
		m_pCaptureWaveFormatX,
		nullptr),
		"Failed to init capture client");

	// Set up the capture Sound buffer
	CR(InitializeCaptureSoundBuffer(numChannels, samplingRate, bufferType));

Error:
	return r;
}

RESULT WASAPISoundClient::Initialize() {
	RESULT r = R_PASS;
	HRESULT hr = S_OK;

	DEBUG_LINEOUT("Initializing WASAPI Sound Client");

	// Enumerate end points 
	// TODO: Member function - allow for better selection
	CR(EnumerateWASAPIDevices());

	//// Initialize the render audio client
	//CRM(InitializeRenderAudioClient(), "Failed to initialize wasapi render");
	
	// Initialize the capture audio client
	CRM(InitializeCaptureAudioClient(), "Failed to initialize wasapi capture");

	// Spatial Audio Client
	//CRM(InitializeSpatialAudioClient() " Failed to initialize wasapi spatial");

Error:
	return r;
}