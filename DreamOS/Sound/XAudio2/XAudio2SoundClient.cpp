#include "XAudio2SoundClient.h"

#include <string>

#include "Sound/SoundFile.h"
#include "Sound/AudioPacket.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"

#include "X3DSpatialSoundObject.h"

XAudio2SoundClient::XAudio2SoundClient(std::wstring *pwstrOptAudioOutputGUID) : 
	SoundClient(pwstrOptAudioOutputGUID)
{
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

	CR((RESULT)m_pXAudio2SourceVoiceStereoFloat32->Start(0));
	CR((RESULT)m_pXAudio2SourceVoiceStereoSignedInt16->Start(0));

	for(int i = 0; i < m_numMonoChannels; i++) {
		if (m_xaudio2MonoSignedInt16Sources[i] != nullptr) {
			CR((RESULT)m_xaudio2MonoSignedInt16Sources[i]->Start(0));
		}
	}

Error:

	DEBUG_LINEOUT("XAudio2SoundClient: AudioRenderProcess End (expected)");

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
	if (m_wstrAudioOutputDeviceGUID.compare(L"default") == 0) {
		CRM((RESULT)m_pXAudio2->CreateMasteringVoice(&pXAudio2MasterVoice, 2, 48000), 
			"Failed to create default XAudio2 Master Voice");
	}
	else {
		CRM((RESULT)m_pXAudio2->CreateMasteringVoice(&pXAudio2MasterVoice, 2, 48000, NULL, m_wstrAudioOutputDeviceGUID.c_str()), 
			"Failed to create XAudio2 Master Voice");
	}

	CNM(pXAudio2MasterVoice, "Failed to allocate XAudio2 master voice");
	m_pXAudio2MasterVoice = std::shared_ptr<IXAudio2MasteringVoice>(pXAudio2MasterVoice);

	// Source voices
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
	m_pXAudio2SourceVoiceStereoFloat32 = std::shared_ptr<IXAudio2SourceVoice>(pXAudio2SourceVoice);
	//*/

	///*
	// Move this to member etc
	// Spatial audio is more restrictive (mono)
	//WAVEFORMATEX sourceFormat;
	sourceFormat.wFormatTag = WAVE_FORMAT_PCM;
	sourceFormat.wBitsPerSample = 16;
	sourceFormat.nChannels = 2;
	sourceFormat.nSamplesPerSec = 48000;
	sourceFormat.nBlockAlign = (sourceFormat.wBitsPerSample >> 3) * sourceFormat.nChannels;
	sourceFormat.nAvgBytesPerSec = sourceFormat.nBlockAlign * sourceFormat.nSamplesPerSec;
	sourceFormat.cbSize = 0;

	CRM((RESULT)m_pXAudio2->CreateSourceVoice(&pXAudio2SourceVoice, (WAVEFORMATEX*)&sourceFormat), "Failed to create source voice");
	CNM(pXAudio2SourceVoice, "Failed to allocate source voice");
	m_pXAudio2SourceVoiceStereoSignedInt16 = std::shared_ptr<IXAudio2SourceVoice>(pXAudio2SourceVoice);
	//*/

	///*
	// Mono channels
	// Move this to member etc
	// Spatial audio is more restrictive (mono)
	//WAVEFORMATEX sourceFormat;
	sourceFormat.wFormatTag = WAVE_FORMAT_PCM;
	sourceFormat.wBitsPerSample = 16;
	sourceFormat.nChannels = 1;
	sourceFormat.nSamplesPerSec = 48000;
	sourceFormat.nBlockAlign = (sourceFormat.wBitsPerSample >> 3) * sourceFormat.nChannels;
	sourceFormat.nAvgBytesPerSec = sourceFormat.nBlockAlign * sourceFormat.nSamplesPerSec;
	sourceFormat.cbSize = 0;

	for (int i = 0; i < m_numMonoChannels; i++) {
		CRM((RESULT)m_pXAudio2->CreateSourceVoice(&pXAudio2SourceVoice, (WAVEFORMATEX*)&sourceFormat), "Failed to create source voice");
		CNM(pXAudio2SourceVoice, "Failed to allocate source voice");
		m_xaudio2MonoSignedInt16Sources[i] = std::shared_ptr<IXAudio2SourceVoice>(pXAudio2SourceVoice);
	}
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
		std::make_shared<X3DSpatialSoundObject>(GetSpaitalSamplingRate(), ptPosition, vEmitterDirection, vListenerDirection, m_pXAudio2, m_pXAudio2MasterVoice);
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
	
	CN(m_pXAudio2SourceVoiceStereoFloat32);

	// TODO: Temporary workaround to play this sound now and flush the rest of the queue
	CRM((RESULT)m_pXAudio2SourceVoiceStereoFloat32->FlushSourceBuffers(), "Failed to flush buffer");

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

		CRM((RESULT)m_pXAudio2SourceVoiceStereoFloat32->SubmitSourceBuffer(&xAudio2SoundBuffer), "Failed to submit source buffer");
	}

Error:
	return r;
}

RESULT XAudio2SoundClient::PushAudioPacket(const AudioPacket &pendingAudioPacket) {
	RESULT r = R_PASS;

	CN(m_pXAudio2SourceVoiceStereoSignedInt16);

	{
		//size_t pAudioBuffer_n = pendingAudioPacket.GetByteSize();
		//int16_t *pAudioBuffer = (int16_t*)pendingAudioPacket.GetDataBuffer();
		
		uint8_t* pAudioBuffer = nullptr;
		size_t pAudioBuffer_n = 0;

		CR(pendingAudioPacket.GetDataBufferCopy(pAudioBuffer, pAudioBuffer_n));
	
		XAUDIO2_BUFFER xAudio2SoundBuffer = { 0 };
		BYTE *pByteAudioBuffer = reinterpret_cast<BYTE*>(pAudioBuffer);
	
		CN(pByteAudioBuffer);
	
		xAudio2SoundBuffer.AudioBytes = (UINT32)pAudioBuffer_n;  //size of the audio buffer in bytes
		xAudio2SoundBuffer.pAudioData = pByteAudioBuffer;  //buffer containing audio data
		xAudio2SoundBuffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
	
		//XAUDIO2_VOICE_STATE voiceState;
		//m_pXAudio2SourceVoiceStereoSignedInt16->GetState(&voiceState, NULL);

		//m_pXAudio2SourceVoiceStereoSignedInt16->FlushSourceBuffers();

//		DEBUG_LINEOUT("%d frames", pendingAudioPacket.GetNumFrames());

		CRM((RESULT)m_pXAudio2SourceVoiceStereoSignedInt16->SubmitSourceBuffer(&xAudio2SoundBuffer), "Failed to submit source buffer");

		// Measure time diff
		static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		auto diffVal = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count();
		lastUpdateTime = timeNow;
	}

Error:
	return r;
}

RESULT XAudio2SoundClient::PlayAudioPacketSigned16Bit(const AudioPacket &pendingAudioPacket, std::string strAudioTrackLabel, int channel) {
	RESULT r = R_PASS;

	CN(m_xaudio2MonoSignedInt16Sources[channel]);

	{
		//size_t pAudioBuffer_n = pendingAudioPacket.GetByteSize();
		//int16_t *pAudioBuffer = (int16_t*)pendingAudioPacket.GetDataBuffer();

		uint8_t* pAudioBuffer = nullptr;
		size_t pAudioBuffer_n = 0;

		CR(pendingAudioPacket.GetDataBufferCopy(pAudioBuffer, pAudioBuffer_n));

		XAUDIO2_BUFFER xAudio2SoundBuffer = { 0 };
		BYTE *pByteAudioBuffer = reinterpret_cast<BYTE*>(pAudioBuffer);

		CN(pByteAudioBuffer);

		xAudio2SoundBuffer.AudioBytes = (UINT32)pAudioBuffer_n;  //size of the audio buffer in bytes
		xAudio2SoundBuffer.pAudioData = pByteAudioBuffer;  //buffer containing audio data
		xAudio2SoundBuffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

														  //XAUDIO2_VOICE_STATE voiceState;
														  //m_pXAudio2SourceVoiceStereoSignedInt16->GetState(&voiceState, NULL);

														  //m_pXAudio2SourceVoiceStereoSignedInt16->FlushSourceBuffers();

//		DEBUG_LINEOUT("%d frames", pendingAudioPacket.GetNumFrames());

		CRM((RESULT)m_xaudio2MonoSignedInt16Sources[channel]->SubmitSourceBuffer(&xAudio2SoundBuffer), "Failed to submit source buffer");

		// Measure time diff
		static std::chrono::system_clock::time_point lastUpdateTime = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		auto diffVal = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count();
		lastUpdateTime = timeNow;
	}

Error:
	return r;
}