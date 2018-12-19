#include "DreamSoundSystem.h"

#include "DreamOS.h"

#include "Scene/ObjectStoreNode.h"

#include "Sound/SoundClientFactory.h"
#include "Sound/SoundFile.h"
#include "Sound/SpatialSoundObject.h"

DreamSoundSystem::DreamSoundSystem(DreamOS *pDreamOS, void *pContext) :
	DreamModule<DreamSoundSystem>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamSoundSystem::~DreamSoundSystem() {
	Shutdown();
}

RESULT DreamSoundSystem::RegisterObserver(DreamSoundSystem::observer *pObserver) {
	RESULT r = R_PASS;

	CBM((m_pObserver == nullptr), "Observer already set");

	m_pObserver = pObserver;

Error:
	return r;
}

RESULT DreamSoundSystem::UnregisterObserver() {
	RESULT r = R_PASS;

	CNRM((m_pObserver), R_SKIPPED, "Observer already null");

	m_pObserver = nullptr;

Error:
	return r;
}

RESULT DreamSoundSystem::InitializeModule(void *pContext) {
	RESULT r = R_PASS;

	SetName("DreamSoundSystem");
	SetModuleDescription("The Dream Sound System Module");

	std::wstring wstrDeviceOutID;
	//std::wstring wstrDeviceInGUID;

	std::wstring wstrHMDDeviceOutGUID = L"default";
	//std::wstring wstrHMDDeviceInGUID;

	m_pHMD = GetDOS()->GetHMD();

	if (m_pHMD != nullptr) {

		auto deviceType = m_pHMD->GetDeviceType();

		switch (deviceType) {

			case HMDDeviceType::OCULUS: {
				

				CRM(m_pHMD->GetAudioDeviceOutID(wstrDeviceOutID), "Failed to get HMD device out");
				//CRM(m_pHMD->GetAudioDeviceInGUID(wstrDeviceInGUID), "Failed to get HMD device in");

				DEBUG_LINEOUT("out: %S", wstrDeviceOutID.c_str());
				//DEBUG_LINEOUT("in: %S", wstrDeviceInGUID.c_str());

				// Apply 

			} break;

			case HMDDeviceType::VIVE: {
				// TODO:
				
				
			} break;

			case HMDDeviceType::META: {
				// TODO:
			} break;

		}
	}

	{
		// WASAPI Capture Client
		// This can fail - if this is the case then capture will not fire (obviously)
		auto pWASAPICaptureClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI);

		if (pWASAPICaptureClient != nullptr) {
			//m_pWASAPICaptureClient = std::shared_ptr<SoundClient>(pWASAPICaptureClient);
			m_pWASAPICaptureClient = pWASAPICaptureClient;
			CN(m_pWASAPICaptureClient);

			CR(m_pWASAPICaptureClient->RegisterObserver(this));
			CRM(m_pWASAPICaptureClient->StartCapture(), "Failed to start WASAPI Capture");

			// Use the WASAPI enumerator to get the full thing
			wstrHMDDeviceOutGUID = pWASAPICaptureClient->GetDeviceIDFromDeviceID(wstrDeviceOutID);
		}

		// XAudio2 Spatial and Render Client
		// This cannot fail - we need a default audio device, if none exists this will blow up
		auto pXAudio2AudioClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_XAUDIO2, &wstrHMDDeviceOutGUID);
		CNM(pXAudio2AudioClient, "Failed to create XAudio2 Client");
		//m_pXAudio2AudioClient = std::shared_ptr<SoundClient>(pXAudio2AudioClient);
		m_pXAudio2AudioClient = pXAudio2AudioClient;
		CN(m_pXAudio2AudioClient);

		CRM(m_pXAudio2AudioClient->StartSpatial(), "Failed to start spatial for XAudio2");
		CRM(m_pXAudio2AudioClient->StartRender(), "Failed to start render for XAudio2");

		CR(ClearSpatialSoundObjects());

		/*
		{
			// This is BULL
			m_pTestSpatialSoundObject = AddSpatialSoundObject(point(0.0f, 0.0f, 2.0f), vector(), vector());
			CN(m_pTestSpatialSoundObject);

			m_pSoundFile = LoadSoundFile(L"OceanWavesMikeKoenig980635527_48K.wav", SoundFile::type::WAVE);
			CN(m_pSoundFile);

			//m_pTestSpatialSoundObject->LoopSoundFile(m_pSoundFile.get());
			m_pTestSpatialSoundObject->PlaySoundFile(m_pSoundFile.get());
		}
		//*/
	}

	// Named Pipe Server
	CR(InitializeNamedPipeServer());

Error:
	return r;
}

const wchar_t kDreamSoundSystemNamedPipeServerName[] = L"dreamsoundsystempipe";

RESULT DreamSoundSystem::HandleServerPipeMessage(void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	char *pszMessage = (char *)(pBuffer);
	CN(pszMessage);

	DEBUG_LINEOUT("DreamSoundSystem::HandleServerPipeMessage: %s", pszMessage);

Error:
	return r;
}

RESULT DreamSoundSystem::InitializeNamedPipeServer() {
	RESULT r = R_PASS;

	// Set up named pipe server
	m_pNamedPipeServer = GetDOS()->MakeNamedPipeServer(kDreamSoundSystemNamedPipeServerName);
	CN(m_pNamedPipeServer);

	CRM(m_pNamedPipeServer->RegisterMessageHandler(std::bind(&DreamSoundSystem::HandleServerPipeMessage, this, std::placeholders::_1, std::placeholders::_2)),
		"Failed to register message handler");
	CR(m_pNamedPipeServer->RegisterNamedPipeServerObserver(this));

	CRM(m_pNamedPipeServer->Start(), "Failed to start server");

Error:
	return r;
}

RESULT DreamSoundSystem::OnClientConnect() {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamSoundSystem::OnClientDisconnect() {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamSoundSystem::OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) {
	RESULT r = R_PASS;

	// 
	/* TEMP: (for 3rd person)
	if(GetDOS()->GetSandboxConfiguration().f3rdPersonCamera) {
		auto pAudioBuffer = pCaptureBuffer->GetAudioPacket;
		CN(pAudioBuffer);

		size_t numSamples = pCaptureBuffer->GetNumChannels() * pCaptureBuffer->GetNumFrames();
		float averageAccumulator = 0.0f;

		for (int i = 0; i < numSamples; ++i) {
			int16_t value = *(static_cast<const int16_t*>(pAudioBuffer) + i);
			float scaledValue = (float)(value) / (std::numeric_limits<int16_t>::max());
			averageAccumulator += std::abs(scaledValue);
		}

		float runTimeAverage = averageAccumulator / numSamples;
		GetDOS()->GetCloudController()->SetRunTimeMicAverage(runTimeAverage);
	}
	//*/

	if (m_pNamedPipeServer != nullptr) {
		void *pDataBuffer = nullptr;
		size_t pDataBuffer_n = 0;

		CR(pCaptureBuffer->GetInterlacedAudioDataBuffer(numFrames, pDataBuffer, pDataBuffer_n, false));
		m_pNamedPipeServer->SendMessage((void*)(pDataBuffer), pDataBuffer_n);
	}

	if (m_pObserver) {
		CRM(m_pObserver->OnAudioDataCaptured(numFrames, pCaptureBuffer), "OnAudioDataCaptured in observer failed");
	}

	

Error:
	return r;
}

RESULT DreamSoundSystem::AddSpatialSoundObject(std::shared_ptr<SpatialSoundObject> pSpatialSoundObject) {
	RESULT r = R_PASS;

	CN(pSpatialSoundObject);

	// Add to our spatial objects
	m_pSpatialSoundObjects.push_back(pSpatialSoundObject);

Error:
	return r;
}

RESULT DreamSoundSystem::RemoveSpatialSoundObject(std::shared_ptr<SpatialSoundObject> pSpatialSoundObject) {
	RESULT r = R_PASS;

	auto findIterator = std::find(m_pSpatialSoundObjects.begin(), m_pSpatialSoundObjects.end(), pSpatialSoundObject);

	if (findIterator != m_pSpatialSoundObjects.end()) {
		m_pSpatialSoundObjects.erase(findIterator);
	}
	else {
		r = R_NOT_FOUND;
	}

Error:
	return r;
}

RESULT DreamSoundSystem::ClearSpatialSoundObjects() {
	// Set up the spatial sound object store
	m_pSpatialSoundObjects = std::vector<std::shared_ptr<SpatialSoundObject>>();

	return R_PASS;
}

std::shared_ptr<SpatialSoundObject> DreamSoundSystem::AddSpatialSoundObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) {
	RESULT r = R_PASS;

	std::shared_ptr<SpatialSoundObject> pSpatialSoundObject = nullptr;

	CNM(m_pXAudio2AudioClient, "XAudio2 client not initialized");
		
	pSpatialSoundObject = m_pXAudio2AudioClient->AddSpatialSoundObject(ptPosition, vEmitterDirection, vListenerDirection);
	CNM(pSpatialSoundObject, "Failed to create spatial sound object");

	// Set camera for object
	CRM(pSpatialSoundObject->SetListenerCamera(GetDOS()->GetCamera()), "Failed to set spatial object camera");

	CR(AddSpatialSoundObject(pSpatialSoundObject));

	return pSpatialSoundObject;

Error:
	if (pSpatialSoundObject != nullptr) {
			pSpatialSoundObject = nullptr;
	}
	return nullptr;
}

std::shared_ptr<SoundFile> DreamSoundSystem::LoadSoundFile(const std::wstring &wstrFilename, SoundFile::type soundFileType) {
	RESULT r = R_PASS;

	SoundFile *pNewSoundFile = SoundFile::LoadSoundFile(wstrFilename.c_str(), soundFileType);
	CN(pNewSoundFile);

	return std::shared_ptr<SoundFile>(pNewSoundFile);

Error:
	if (pNewSoundFile != nullptr) {
		delete pNewSoundFile;
		pNewSoundFile = nullptr;
	}

	return nullptr;
}

RESULT DreamSoundSystem::PlayAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	CN(m_pXAudio2AudioClient);

	CRM(m_pXAudio2AudioClient->PushAudioPacket(pendingAudioPacket), "Failed to play audio packet");

Error:
	return r;
}

RESULT DreamSoundSystem::PlayAudioPacketSigned16Bit(const AudioPacket &pendingAudioPacket, std::string strAudioTrackLabel, int channel) {
	RESULT r = R_PASS;

	CN(m_pXAudio2AudioClient);

	// TODO: Would be way cooler to allow DSS to create it's own channels, own them, and pass audio to them
	CRM(m_pXAudio2AudioClient->PlayAudioPacketSigned16Bit(pendingAudioPacket, strAudioTrackLabel, channel), "Failed to play audio packet");

Error:
	return r;
}

RESULT DreamSoundSystem::LoopSoundFile(std::shared_ptr<SoundFile> pSoundFile) {
	RESULT r = R_PASS;

	CN(m_pXAudio2AudioClient);
	CN(pSoundFile);

	CRM(m_pXAudio2AudioClient->LoopSoundFile(pSoundFile.get()), "Failed to loop sound file");

Error:
	return r;
}

RESULT DreamSoundSystem::PlaySoundFile(std::shared_ptr<SoundFile> pSoundFile) {
	RESULT r = R_PASS;

	CN(m_pXAudio2AudioClient);
	CN(pSoundFile);

	CRM(m_pXAudio2AudioClient->PlaySoundFile(pSoundFile.get()), "Failed to play sound file");

Error:
	return r;
}

RESULT DreamSoundSystem::Update(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

	// TODO: Update all spatial objects with regards to HMD orientation / position etc

Error:
	return r;
}

RESULT DreamSoundSystem::OnDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamSoundSystem::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	m_pWASAPICaptureClient->StopCapture();
	CR(r);

Error:
	return r;
}

// The Self Construct
DreamSoundSystem* DreamSoundSystem::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamSoundSystem *pDreamModule = new DreamSoundSystem(pDreamOS, pContext);
	return pDreamModule;
}

float DreamSoundSystem::GetRunTimeCaptureAverage() {
	if (m_pWASAPICaptureClient != nullptr) {
		return m_pWASAPICaptureClient->GetRunTimeCaptureAverage();
	}

	return 0.0f;
}