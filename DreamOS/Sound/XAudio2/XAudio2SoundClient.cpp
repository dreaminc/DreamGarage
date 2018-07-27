#include "XAudio2SoundClient.h"

#include <string>

#include "Sound/SoundFile.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"

#include "X3DSpatialSoundObject.h"

XAudio2SoundClient::XAudio2SoundClient() {
	// empty
}

XAudio2SoundClient::~XAudio2SoundClient() {
	if (m_pXAudio2MasterVoice != nullptr) {
		m_pXAudio2MasterVoice->DestroyVoice();
		m_pXAudio2MasterVoice = nullptr;
	}

	if (m_pXAudio2 != nullptr) {
		m_pXAudio2 = nullptr;
	}
}

// TODO:
RESULT XAudio2SoundClient::AudioSpatialProcess() {
	return R_NOT_IMPLEMENTED;
}

// TODO:
RESULT XAudio2SoundClient::AudioCaptureProcess() {
	return R_NOT_IMPLEMENTED;
}

// TODO:
RESULT XAudio2SoundClient::AudioRenderProcess() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("XAudio2SoundClient: AudioRenderProcess Start");

	CR((RESULT)m_pXAudio2SourceVoice->Start(0));

Error:

	DEBUG_LINEOUT("XAudio2SoundClient: AudioRenderProcess End");

	return r;
}

// TODO:
RESULT XAudio2SoundClient::Initialize() {
	RESULT r = R_PASS;
	HRESULT hr = S_OK;

	DEBUG_LINEOUT("Initializing XAudio2 Sound Client");	

	//// Spatial Audio Client
	//CR(InitializeSpatialAudioClient());
	
	// Set up engine
	IXAudio2* pXAudio2 = nullptr;
	CRM((RESULT)XAudio2Create(&pXAudio2, XAUDIO2_1024_QUANTUM, XAUDIO2_DEFAULT_PROCESSOR), "Failed to create XAudio2 Engine");
	CNM(pXAudio2, "Failed to allocate XAudio2 engine");
	m_pXAudio2 = std::shared_ptr<IXAudio2>(pXAudio2);

	// Set up master voice
	IXAudio2MasteringVoice* pXAudio2MasterVoice = nullptr;
	CRM((RESULT)m_pXAudio2->CreateMasteringVoice(&pXAudio2MasterVoice, 2, 48000), "Failed to create XAudio2 Master Voice");
	CNM(pXAudio2MasterVoice, "Failed to allocate XAudio2 master voice");
	m_pXAudio2MasterVoice = std::shared_ptr<IXAudio2MasteringVoice>(pXAudio2MasterVoice);

	// Source voice
	///*
	// Move this to member etc
	// Spatial audio is more restrictive (mono)
	WAVEFORMATEX sourceFormat;
	sourceFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	sourceFormat.wBitsPerSample = 32;
	sourceFormat.nChannels = 2;
	sourceFormat.nSamplesPerSec = 48000;
	sourceFormat.nBlockAlign = (sourceFormat.wBitsPerSample >> 3) * sourceFormat.nChannels;
	sourceFormat.nAvgBytesPerSec = sourceFormat.nBlockAlign * sourceFormat.nSamplesPerSec;
	sourceFormat.cbSize = 0;

	IXAudio2SourceVoice* pXAudio2SourceVoice;
	CRM((RESULT)m_pXAudio2->CreateSourceVoice(&pXAudio2SourceVoice, (WAVEFORMATEX*)&sourceFormat), "Failed to create source voice");
	CNM(pXAudio2SourceVoice, "Failed to allocate source voice");
	m_pXAudio2SourceVoice = std::shared_ptr<IXAudio2SourceVoice>(pXAudio2SourceVoice);
	//*/

	// Spatial Audio

	DWORD dwChannelMask;
	m_pXAudio2MasterVoice->GetChannelMask(&dwChannelMask);

	m_pX3DInstance = std::shared_ptr<X3DAUDIO_HANDLE>(new X3DAUDIO_HANDLE);
	CNM(m_pX3DInstance, "Failed to allocate X3D instance");

	CRM((RESULT)X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_pX3DInstance.get()),
		"Failed to initialize XAudio 3D");



Error:
	return r;
}

// TODO:
std::shared_ptr<SpatialSoundObject> XAudio2SoundClient::MakeSpatialAudioObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection) {
	RESULT r = R_PASS;

	std::shared_ptr<SpatialSoundObject> pSpatialSoundObject = nullptr;
	
	CNM(m_pXAudio2, "XAudio2 Engine not initialized");
	CNM(m_pXAudio2MasterVoice, "Master voice not initialized");
	CNM(m_pX3DInstance, "X3D Instance not initialized");

	pSpatialSoundObject =
		std::make_shared<X3DSpatialSoundObject>(ptPosition, vEmitterDirection, vListenerDirection, m_pXAudio2, m_pXAudio2MasterVoice);
	CNM(pSpatialSoundObject, "Failed to allocate X3D spatial sound object");

	CRM(pSpatialSoundObject->Initialize(), "Failed to initialize X3D HRTF spatial object");

	return pSpatialSoundObject;

Error:
	if (pSpatialSoundObject != nullptr) {
		pSpatialSoundObject = nullptr;
	}

	return nullptr;
}

//RESULT XAudio2SoundClient::InitializeSpatialAudioClient() {
//	RESULT r = R_PASS;
//
//	CR(r);
//
//Error:
//	return r;
//}

RESULT XAudio2SoundClient::PlaySoundFile(SoundFile *pSoundFile) {
	RESULT r = R_PASS;
	
	CN(m_pXAudio2SourceVoice);

	{
		float *pFloatAudioBuffer = nullptr;
		size_t pFloatAudioBuffer_n = 0;

		CR(pSoundFile->GetAudioBuffer(pFloatAudioBuffer));

		// TODO: Blah, sound buffer is a float - this is not general
		pFloatAudioBuffer_n = pSoundFile->GetNumSamples() * sizeof(float);

		XAUDIO2_BUFFER xAudio2SoundBuffer = { 0 };
		BYTE *pByteAudioBuffer = reinterpret_cast<BYTE*>(pFloatAudioBuffer);

		CN(pByteAudioBuffer);

		xAudio2SoundBuffer.AudioBytes = (UINT32)pFloatAudioBuffer_n;  //size of the audio buffer in bytes
		xAudio2SoundBuffer.pAudioData = pByteAudioBuffer;  //buffer containing audio data
		xAudio2SoundBuffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

		CRM((RESULT)m_pXAudio2SourceVoice->SubmitSourceBuffer(&xAudio2SoundBuffer), "Failed to submit source buffer");
	}

Error:
	return r;
}

