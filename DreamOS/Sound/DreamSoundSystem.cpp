#include "DreamSoundSystem.h"

#include "DreamOS.h"

#include "Scene/ObjectStoreNode.h"

#include "SoundClientFactory.h"
#include "SoundFile.h"
#include "SpatialSoundObject.h"

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

	CNM((m_pObserver), "Observer already null");

	m_pObserver = nullptr;

Error:
	return r;
}

RESULT DreamSoundSystem::InitializeModule(void *pContext) {
	RESULT r = R_PASS;

	SetName("DreamSoundSystem");
	SetModuleDescription("The Dream Sound System Module");

	// WASAPI Capture Client
	auto pWASAPICaptureClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_WASAPI);
	CNM(pWASAPICaptureClient, "Failed to create WASAPI Client");
	m_pWASAPICaptureClient = std::shared_ptr<SoundClient>(pWASAPICaptureClient);
	CN(m_pWASAPICaptureClient);

	CR(m_pWASAPICaptureClient->RegisterObserver(this));
	CRM(m_pWASAPICaptureClient->StartCapture(), "Failed to start WASAPI Capture");

	// XAudio2 Spatial and Render Client
	auto pXAudio2AudioClient = SoundClientFactory::MakeSoundClient(SOUND_CLIENT_TYPE::SOUND_CLIENT_XAUDIO2);
	CNM(pXAudio2AudioClient, "Failed to create XAudio2 Client");
	m_pXAudio2AudioClient =std::shared_ptr<SoundClient>(pXAudio2AudioClient);
	CN(m_pXAudio2AudioClient);

	// Set up the spatial sound object store
	m_pSpatialSoundObjectGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pSpatialSoundObjectGraph, "Failed to allocate Scene Graph");

	m_pHMD = GetDOS()->GetHMD();

	/* TODO: This is where we can detect the audio routings to pass to the sound system
	if (pHMD != nullptr) {
		auto deviceType = pHMD->GetDeviceType();

		switch (deviceType) {
		
			case HMDDeviceType::OCULUS: {
				// TODO: 
			} break;
			
			case HMDDeviceType::VIVE: {
				// TODO: 
			} break;
			
			case HMDDeviceType::META: {
				// TODO: 
			} break;
		
		}
	}
	*/

Error:
	return r;
}

RESULT DreamSoundSystem::OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) {
	RESULT r = R_PASS;

	// Don't do much now

	if (m_pObserver) {
		CRM(m_pObserver->OnAudioDataCaptured(numFrames, pCaptureBuffer), "OnAudioDataCaptured in observer failed");
	}

Error:
	return r;
}

std::shared_ptr<SpatialSoundObject> DreamSoundSystem::AddSpatialSoundObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) {
	RESULT r = R_PASS;

	CNM(m_pXAudio2AudioClient, "XAudio2 client not initialized");
	
	{
		std::shared_ptr<SpatialSoundObject> pSpatialSoundObject = m_pXAudio2AudioClient->AddSpatialSoundObject(ptPosition, vEmitterDirection, vListenerDirection);
		CNM(pSpatialSoundObject, "Failed to create spatial sound object");

		return pSpatialSoundObject;
	}

Error:
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

	CR(r);

Error:
	return r;
}

// The Self Construct
DreamSoundSystem* DreamSoundSystem::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamSoundSystem *pDreamModule = new DreamSoundSystem(pDreamOS, pContext);
	return pDreamModule;
}