#include "X3DSpatialSoundObject.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"

#include "Sound/SoundFile.h"

X3DSpatialSoundObject::X3DSpatialSoundObject(point ptOrigin, vector vEmitterDirection, vector vListenerDirection, std::shared_ptr<IXAudio2> pXAudio2, std::shared_ptr<IXAudio2MasteringVoice> pXAudio2MasterVoice) :
	SpatialSoundObject(ptOrigin, vEmitterDirection, vListenerDirection),
	m_pXAudio2(pXAudio2),
	m_pXAudio2MasterVoice(pXAudio2MasterVoice)
{
	// empty
}

X3DSpatialSoundObject::~X3DSpatialSoundObject() {
	Kill();
}

RESULT X3DSpatialSoundObject::Kill() {
	RESULT r = R_PASS;

	if (m_pXAudio2SourceVoice != nullptr) {
		m_pXAudio2SourceVoice = nullptr;
	}

	if (m_hBufferEndEvent != nullptr) {
		CloseHandle(m_hBufferEndEvent);
		m_hBufferEndEvent = nullptr;
	}

//Error:
	return r;
}

RESULT X3DSpatialSoundObject::Initialize() {
	RESULT r = R_PASS;

	// TODO: to members
	float scaling = 1.0f; 
	float order = 2.0f;

	// Activate a new dynamic audio object

	CNM(m_pXAudio2, "XAudio2 Engine not initialized");
	CNM(m_pXAudio2MasterVoice, "Master voice not initialized");
	
	WAVEFORMATEX sourceFormat;
	sourceFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	sourceFormat.wBitsPerSample = 32;
	sourceFormat.nChannels = 1;
	sourceFormat.nSamplesPerSec = 48000;
	sourceFormat.nBlockAlign = (sourceFormat.wBitsPerSample >> 3) * sourceFormat.nChannels;
	sourceFormat.nAvgBytesPerSec = sourceFormat.nBlockAlign * sourceFormat.nSamplesPerSec;
	sourceFormat.cbSize = 0;

	// Create a source voice to accept audio data in the specified format.
	IXAudio2SourceVoice* pXAudio2SourceVoice = nullptr;
	CRM((RESULT)m_pXAudio2->CreateSourceVoice(&pXAudio2SourceVoice, (WAVEFORMATEX*)&sourceFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this, NULL, NULL),
		"Failed to initialize object voice");

	CN(pXAudio2SourceVoice);
	m_pXAudio2SourceVoice = std::shared_ptr<IXAudio2SourceVoice>(pXAudio2SourceVoice);

	// Cardioid directivity configuration
	HrtfDirectivityCardioid hrtfDirectecityCardiod;
	hrtfDirectecityCardiod.directivity.type = HrtfDirectivityType::Cardioid;
	hrtfDirectecityCardiod.directivity.scaling = scaling;
	hrtfDirectecityCardiod.order = order;

	// APO initialization
	HrtfApoInit hrtfAPOInit;
	hrtfAPOInit.directivity = &hrtfDirectecityCardiod.directivity;
	hrtfAPOInit.distanceDecay = nullptr;                // This specifies natural distance decay behavior (simulates real world)

	{
		if (m_pXAPO != nullptr) {
			m_pXAPO.Reset();
		}

		CR((RESULT)CreateHrtfApo(&hrtfAPOInit, &m_pXAPO));
		CN(m_pXAPO);

		if (m_pHRTFParams != nullptr) {
			m_pHRTFParams.Reset();
		}
		
		m_pXAPO.As(&m_pHRTFParams);
		HrtfEnvironment hrtfEnvironment = HrtfEnvironment::Outdoors;

		m_pHRTFParams->SetEnvironment(hrtfEnvironment);

		// Create a submix voice that will host the xAPO - this submix voice will be destroyed when XAudio2 instance is destroyed.
		{
			IXAudio2SubmixVoice* pXAudio2SubmixVoice = nullptr;

			XAUDIO2_EFFECT_DESCRIPTOR xAudioEffectDesc{};
			xAudioEffectDesc.InitialState = TRUE;
			xAudioEffectDesc.OutputChannels = 2;          // Stereo output
			xAudioEffectDesc.pEffect = m_pXAPO.Get();              // HRTF xAPO set as the effect.

			XAUDIO2_EFFECT_CHAIN xAudioEffectChain{};
			xAudioEffectChain.EffectCount = 1;
			xAudioEffectChain.pEffectDescriptors = &xAudioEffectDesc;

			XAUDIO2_VOICE_SENDS xAudioVoiceSends = {};
			XAUDIO2_SEND_DESCRIPTOR xAudioSendDesc = {};
			xAudioSendDesc.pOutputVoice = m_pXAudio2MasterVoice.get();
			xAudioVoiceSends.SendCount = 1;
			xAudioVoiceSends.pSends = &xAudioSendDesc;

			// HRTF APO expects mono 48kHz input, so we configure the submix voice for that format.
			CRM((RESULT)m_pXAudio2->CreateSubmixVoice(&pXAudio2SubmixVoice, 1, 48000, 0, 0, &xAudioVoiceSends, &xAudioEffectChain),
				"Failed to create submix voice");
			CNM(pXAudio2SubmixVoice, "Failed to allocate submix voice");
			m_pXAudio2SubmixVoice = std::shared_ptr<IXAudio2SubmixVoice>(pXAudio2SubmixVoice);
		}

		// Route the source voice to the submix voice.
		// The complete graph pipeline looks like this - Source Voice -> Submix Voice (HRTF xAPO) -> Mastering Voice
		{
			XAUDIO2_VOICE_SENDS xAudioVoiceSends = {};
			XAUDIO2_SEND_DESCRIPTOR xAudioSendDesc = {};
			xAudioSendDesc.pOutputVoice = m_pXAudio2SubmixVoice.get();
			xAudioVoiceSends.SendCount = 1;
			xAudioVoiceSends.pSends = &xAudioSendDesc;

			CRM((RESULT)pXAudio2SourceVoice->SetOutputVoices(&xAudioVoiceSends), "Failed to set output of source voice to submix");
		}
	}

	m_hBufferEndEvent = CreateEvent(nullptr, false, false, nullptr);
	CNM(m_hBufferEndEvent, "Failed to create buffer end event for object");

	// XAudio manages the buffers for us, so we don't need to do this apparently 
	//CRM(InitializeSoundBuffer(1, SoundBuffer::type::FLOATING_POINT_32_BIT),
	//	"Failed to initialize sound buffer for spatial audio object");

	CRM((RESULT)m_pXAudio2SourceVoice->Start(0), "Failed to start spatial object voice");

Error:
	return r;
}

RESULT X3DSpatialSoundObject::LoadSoundFile(SoundFile *pSoundFile) {
	return R_NOT_IMPLEMENTED;
}

RESULT X3DSpatialSoundObject::PushMonoAudioBuffer(int numFrames, SoundBuffer *pSoundBuffer) {
	RESULT r = R_PASS;

	m_fLoop = false;

	float *pFloatAudioBuffer = nullptr;

	CN(pSoundBuffer);

	pFloatAudioBuffer = (float*)malloc(sizeof(float) * numFrames);
	CN(pFloatAudioBuffer);

	CR(pSoundBuffer->LoadDataToInterlacedTargetBuffer(pFloatAudioBuffer, numFrames));

	{
		XAUDIO2_BUFFER xaudio2AudioBuffer{};

		xaudio2AudioBuffer.AudioBytes = static_cast<UINT32>(sizeof(float) * numFrames);
		xaudio2AudioBuffer.pAudioData = reinterpret_cast<BYTE*>(pFloatAudioBuffer);
		xaudio2AudioBuffer.pContext = reinterpret_cast<void*>(pFloatAudioBuffer);

		CRM((RESULT)m_pXAudio2SourceVoice->SubmitSourceBuffer(&xaudio2AudioBuffer), "Failed to submit source buffer");
	}

Error:
	// The buffer will be deleted in the buffer-end handler

	return r;
}

// More control is available using the callbacks if needed in the future
RESULT X3DSpatialSoundObject::LoopSoundFile(SoundFile *pSoundFile) {
	RESULT r = R_PASS;

	m_fLoop = true;

	float *pFloatAudioBuffer = nullptr;

	CN(pSoundFile);
	CR(pSoundFile->GetAudioBuffer(pFloatAudioBuffer, 1));

	{
		XAUDIO2_BUFFER xaudio2AudioBuffer{};

		xaudio2AudioBuffer.AudioBytes = static_cast<UINT32>(sizeof(float) * pSoundFile->GetNumFrames());
		xaudio2AudioBuffer.pAudioData = reinterpret_cast<BYTE*>(pFloatAudioBuffer);
		xaudio2AudioBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		xaudio2AudioBuffer.pContext = reinterpret_cast<void*>(pFloatAudioBuffer);
		CRM((RESULT)m_pXAudio2SourceVoice->SubmitSourceBuffer(&xaudio2AudioBuffer), "Failed to submit source buffer");
	}

Error:
	// The buffer will be deleted in the buffer-end handler

	return r;
}

RESULT X3DSpatialSoundObject::PlaySoundFile(SoundFile *pSoundFile) {
	RESULT r = R_PASS;

	m_fLoop = false;

	float *pFloatAudioBuffer = nullptr;
	
	CN(pSoundFile);
	CR(pSoundFile->GetAudioBuffer(pFloatAudioBuffer, 1));
	
	{
		XAUDIO2_BUFFER xaudio2AudioBuffer{};
	
		xaudio2AudioBuffer.AudioBytes = static_cast<UINT32>(sizeof(float) * pSoundFile->GetNumFrames());
		xaudio2AudioBuffer.pAudioData = reinterpret_cast<BYTE*>(pFloatAudioBuffer);
		xaudio2AudioBuffer.pContext = reinterpret_cast<void*>(pFloatAudioBuffer);
		CRM((RESULT)m_pXAudio2SourceVoice->SubmitSourceBuffer(&xaudio2AudioBuffer), "Failed to submit source buffer");
	}

Error:
	// The buffer will be deleted in the buffer-end handler

	return r;
}

RESULT X3DSpatialSoundObject::Update(unsigned int numFrames, unsigned int numChannels) {
	RESULT r = R_PASS;

	point ptPosition = GetPosition(true);

	CN(m_pHRTFParams);

	auto hrtfPosition = HrtfPosition{ ptPosition.x(), ptPosition.y(), -ptPosition.z() };
	CR((RESULT)m_pHRTFParams->SetSourcePosition(&hrtfPosition));

	//auto sourceOrientation = OrientationFromAngles(pitch, yaw, roll);
	//hr = _hrtfParams->SetSourceOrientation(&sourceOrientation);

Error:
	return r;
}

void X3DSpatialSoundObject::OnStreamEnd() {
	//int a = 5;
}

void X3DSpatialSoundObject::OnVoiceProcessingPassEnd() {
	//int a = 5;
}

void X3DSpatialSoundObject::OnVoiceProcessingPassStart(UINT32 bytesRequired) {
	Update(bytesRequired, 1);
}

void X3DSpatialSoundObject::OnBufferEnd(void *pBufferContext) {

	// We create these static buffers when we submit a packet of audio
	// so here we de-allocate them

	if (pBufferContext != nullptr) {
		float *pFloatAudioBuffer = reinterpret_cast<float*>(pBufferContext);

		if (pFloatAudioBuffer != nullptr) {
			delete[] pFloatAudioBuffer;
			pFloatAudioBuffer = nullptr;
		}
	}
}

void X3DSpatialSoundObject::OnBufferStart(void *pBufferContext) {
	//int a = 5;
}

void X3DSpatialSoundObject::OnLoopEnd(void *pBufferContext) {
	//int a = 5;
}

void X3DSpatialSoundObject::OnVoiceError(void *pBufferContext, HRESULT hrError) {
	//int a = 5;
}