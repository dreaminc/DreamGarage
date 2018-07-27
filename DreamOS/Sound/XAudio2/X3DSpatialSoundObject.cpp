#include "X3DSpatialSoundObject.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"

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
	CRM((RESULT)m_pXAudio2->CreateSourceVoice(&pXAudio2SourceVoice, (WAVEFORMATEX*)&sourceFormat), "Failed to initialize object voice");
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

		if (m_pHRTFfParams != nullptr) {
			m_pHRTFfParams.Reset();
		}
		
		m_pXAPO.As(&m_pHRTFfParams);
		HrtfEnvironment hrtfEnvironment = HrtfEnvironment::Outdoors;

		m_pHRTFfParams->SetEnvironment(hrtfEnvironment);

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

Error:
	return r;
}

RESULT X3DSpatialSoundObject::Update(unsigned int numFrames, unsigned int numChannels) {
	RESULT r = R_PASS;

	//point ptPosition = GetPosition(true);
	//CR((RESULT)m_pSpatialAudioObjectHRTF->SetPosition(ptPosition.x(), ptPosition.y(), ptPosition.z()));
	//
	//// TODO: Get from virtual object and camera etc
	//CR(UpdateSpatialSoundObjectOrientation());
	//
	//// Copy data into buffer
	//CR(LoadDataFromBuffer(numFrames, numChannels));

	// TODO:

	CR(r);

Error:
	return r;
}

/*
RESULT X3DSpatialSoundObject::GetBuffer(BYTE **ppBuffer, UINT32 *pBufferLength) {
	RESULT r = R_PASS;

	CRM((RESULT)m_pSpatialAudioObjectHRTF->GetBuffer(ppBuffer, pBufferLength), "Failed to get buffer");

Error:
	return r;
}

RESULT X3DSpatialSoundObject::LoadDataFromBuffer(unsigned int numFrames, unsigned int numChannels) {
	RESULT r = R_PASS;

	BYTE *pBuffer = nullptr;
	UINT32 pBuffer_n = 0;

	CN(m_pSoundBuffer);

	CR(GetBuffer(&pBuffer, &pBuffer_n));

	float *pDataBuffer = reinterpret_cast<float*>(pBuffer);
	
	size_t numFramesInBuffer = m_pSoundBuffer->NumPendingBytes();
	size_t numFramesRead = 0;

	while (numFramesRead < numFrames) {

		if (m_fLoop) {
			if (numFramesInBuffer <= numFrames) {
				CR(m_pSoundBuffer->LoadDataToInterlacedTargetBuffer(pDataBuffer, (int)numFramesInBuffer));
				m_pSoundBuffer->ResetBuffer(m_startLoop, m_endLoop);

				numFramesRead += numFramesInBuffer;
			}
			else {
				CR(m_pSoundBuffer->LoadDataToInterlacedTargetBuffer(pDataBuffer, numFrames));
				numFramesRead += numFrames;
			}
		}
		else {
			CR(m_pSoundBuffer->LoadDataToInterlacedTargetBuffer(pDataBuffer, numFrames));
			numFramesRead += numFrames;
		}
	}

Error:
	return r;
}

RESULT X3DSpatialSoundObject::WriteTestSignalToAudioObjectBuffer(unsigned int numFrames, unsigned int samplingRate, unsigned int numChannels, float frequency) {
	RESULT r = R_PASS;

	static double theta = 0.0f;

	BYTE *pBuffer = nullptr;
	UINT32 pBuffer_n = 0;

	CR(GetBuffer(&pBuffer, &pBuffer_n));

	float *pDataBuffer = reinterpret_cast<float*>(pBuffer);

	for (unsigned int i = 0; i < (numFrames * numChannels); i += numChannels) {

		float val = sin(theta);

		//val *= 0.25f;

		for (unsigned int j = 0; j < numChannels; j++) {
			pDataBuffer[i + j] = val;
		}

		// Increment theta
		theta += ((2.0f * M_PI) / (float)(samplingRate)) * frequency;

		if (theta >= 2.0f * M_PI) {
			theta = theta - (2.0f * M_PI);
		}
	}

Error:
	return r;
}

// TODO: Decompose using our matrix libs
DirectX::XMMATRIX CalculateEmitterConeOrientationMatrix(Windows::Foundation::Numerics::float3 listenerOrientationFront, Windows::Foundation::Numerics::float3 emitterDirection) {

	DirectX::XMVECTOR vListenerDirection = DirectX::XMLoadFloat3(&listenerOrientationFront);
	DirectX::XMVECTOR vEmitterDirection = DirectX::XMLoadFloat3(&emitterDirection);
	DirectX::XMVECTOR vCross = DirectX::XMVector3Cross(vListenerDirection, vEmitterDirection);
	DirectX::XMVECTOR vDot = DirectX::XMVector3Dot(vListenerDirection, vEmitterDirection);
	DirectX::XMVECTOR vAngle = DirectX::XMVectorACos(vDot);

	float angle = DirectX::XMVectorGetX(vAngle);

	// The angle must be non-zero
	if (fabsf(angle) > FLT_EPSILON) {

		// And less than PI
		if (fabsf(angle) < DirectX::XM_PI) {
			return DirectX::XMMatrixRotationAxis(vCross, angle);
		}
		else {
			// If equal to PI, find any other non-collinear vector to generate the perpendicular vector to rotate about

			DirectX::XMFLOAT3 vector = { 1.0f, 1.0f, 1.0f };

			if (listenerOrientationFront.x != 0.0f) {
				vector.x = -listenerOrientationFront.x;
			}
			else if (listenerOrientationFront.y != 0.0f) {
				vector.y = -listenerOrientationFront.y;
			}
			else {
				vector.z = -listenerOrientationFront.z;
			}

			DirectX::XMVECTOR vVector = DirectX::XMLoadFloat3(&vector);

			vVector = DirectX::XMVector3Normalize(vVector);
			vCross = DirectX::XMVector3Cross(vVector, vEmitterDirection);

			return DirectX::XMMatrixRotationAxis(vCross, angle);
		}
	}

	// If the angle is zero, use an identity matrix
	return DirectX::XMMatrixIdentity();
}

// TODO: Decompose using our matrix libs
RESULT X3DSpatialSoundObject::UpdateSpatialSoundObjectOrientation() {
	RESULT r = R_PASS;

	Windows::Foundation::Numerics::float3 emitterDirection = 
		Windows::Foundation::Numerics::float3(m_vEmitterDirection.x(), m_vEmitterDirection.y(), m_vEmitterDirection.z());

	Windows::Foundation::Numerics::float3 listenerDirection = 
		Windows::Foundation::Numerics::float3(m_vListenerDirection.x(), m_vListenerDirection.y(), m_vListenerDirection.z());

	DirectX::XMFLOAT4X4 rotationMatrix;

	DirectX::XMMATRIX rotation = CalculateEmitterConeOrientationMatrix(emitterDirection, listenerDirection);

	XMStoreFloat4x4(&rotationMatrix, rotation);

	SpatialAudioHrtfOrientation spatialAudioHRTFOrientationMatrix = {
		rotationMatrix._11, rotationMatrix._12, rotationMatrix._13,
		rotationMatrix._21, rotationMatrix._22, rotationMatrix._23,
		rotationMatrix._31, rotationMatrix._32, rotationMatrix._33
	};

	CR((RESULT)m_pSpatialAudioObjectHRTF->SetOrientation(&spatialAudioHRTFOrientationMatrix));

Error:
	return r;
}
*/