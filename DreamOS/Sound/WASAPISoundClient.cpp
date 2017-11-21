#include "WASAPISoundClient.h"

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#include <string>


// File specific utilities
template <class T> RESULT SafeRelease(T **ppT) {
	RESULT r = R_PASS;

	CN(*ppT);

	(*ppT)->Release();
	*ppT = nullptr;

Error:
	return r;
}

WASAPISoundClient::WASAPISoundClient() {
	// empty
}

WASAPISoundClient::~WASAPISoundClient() {
	// empty
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
	CRM((RESULT)m_pAudioEndpointDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&m_pSessionManager), "Failed to activate session manager 2 interface");
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

std::wstring WASAPISoundClient::GetDeviceName(IMMDeviceCollection *pDeviceCollection, UINT DeviceIndex) {
	RESULT r = R_PASS;

	IMMDevice *pDevice;
	LPWSTR deviceId;
	std::wstring wstrResult = L"";

	CR((RESULT)pDeviceCollection->Item(DeviceIndex, &pDevice));
	CN(pDevice);

	CR((RESULT)pDevice->GetId(&deviceId));

	IPropertyStore *pPropertyStore;
	CR((RESULT)pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore));
	
	SafeRelease(&pDevice);

	PROPVARIANT propVariantFriendlyName;
	PropVariantInit(&propVariantFriendlyName);
	
	CR((RESULT)pPropertyStore->GetValue(PKEY_Device_FriendlyName, &propVariantFriendlyName));

	SafeRelease(&pPropertyStore);

	wstrResult = std::wstring(propVariantFriendlyName.pwszVal);

	PropVariantClear(&propVariantFriendlyName);
	CoTaskMemFree(deviceId);

Error:
	return wstrResult;
}

// TODO: This simply enumerates the devices but doesn't select one
RESULT WASAPISoundClient::EnumerateWASAPIDevices() {
	RESULT r = R_PASS;
	HRESULT hr = S_OK;

	// TODO: Do we want to keep the collection as member?
	IMMDeviceCollection *pDeviceCollection = nullptr;
	IMMDevice *pDevice = nullptr;

	CRM((RESULT)CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_pEnumerator),
		"CoCreateInstance Failed");
	CN(m_pEnumerator);

	CR((RESULT)m_pEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &pDeviceCollection));

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

RESULT WASAPISoundClient::AudioProcess() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("WASAPISoundClient::AudioProcess Start");

	// Implement the audio loop here
	CR(r);

	DEBUG_LINEOUT("WASAPISoundClient::AudioProcess Finish");

Error:
	return r;
}

RESULT WASAPISoundClient::InitializeAudioClient() {
	RESULT r = R_PASS;

	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
	//REFERENCE_TIME hnsActualDuration;

	// Default Endpoint
	CRM((RESULT)m_pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_pAudioEndpointDevice), "Failed to get default audio endpoint");
	CN(m_pAudioEndpointDevice);

	// Audio Client Device
	CRM((RESULT)m_pAudioEndpointDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_pAudioClient), "Failed to active audio client");
	CN(m_pAudioClient);

	CR((RESULT)m_pAudioClient->GetMixFormat(&m_pWaveFormatX));
	CN(m_pWaveFormatX);

	// Initialize Audio Client
	CRM((RESULT)m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, m_pWaveFormatX, nullptr), "Failed to init client");

Error:
	return r;
}

RESULT WASAPISoundClient::Initialize() {
	RESULT r = R_PASS;
	HRESULT hr = S_OK;

	//UINT32 bufferFrameCount;
	//UINT32 numFramesAvailable;
	//UINT32 packetLength = 0;

	//BYTE *pData = nullptr;

	//DWORD captureFlags;

	DEBUG_LINEOUT("Initializing WASAPI Sound Client");

	// Enumerate end points 
	// TODO: Member function - allow for better selection
	CR(EnumerateWASAPIDevices());

	
	//CR(EnumerateWASAPISessions(pSessionManager));

	// Initialize the audio client
	CR(InitializeAudioClient());
	

	// Test: Try to play something



	/*
	// This is for recording stuff
	CRM((RESULT)pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient), "Failed to get service");
	
	// Notify the audio sink which format to use.
	//CR(pAudioSink->SetFormat(pwfx));

	// Calculate the actual duration of the allocated buffer.
	hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

	// Start recording.
	CR((RESULT)pAudioClient->Start());  

	// Each loop fills about half of the shared buffer.
	while (fDone == false) {

		// Sleep for half the buffer duration.
		Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

		CR((RESULT)pCaptureClient->GetNextPacketSize(&packetLength));

		while (packetLength > 0) {
			// Get the available data in the shared buffer.
			CR((RESULT)pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &captureFlags, nullptr, nullptr));

			if (captureFlags & AUDCLNT_BUFFERFLAGS_SILENT) {
				pData = nullptr;  // Tell CopyData to write silence.
			}

			// Copy the available capture data to the audio sink.
			//CR(pMySink->CopyData(pData, numFramesAvailable, &bDone));

			CR((RESULT)pCaptureClient->ReleaseBuffer(numFramesAvailable));

			CR((RESULT)pCaptureClient->GetNextPacketSize(&packetLength));
		}
	}

	// Stop recording.
	CR((RESULT)pAudioClient->Stop());
	*/

Error:
	return r;
}