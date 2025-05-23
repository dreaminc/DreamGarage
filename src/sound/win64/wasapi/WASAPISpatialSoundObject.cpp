#include "WASAPISpatialSoundObject.h"

#include <DirectXMath.h>
#include <WindowsNumerics.h>

#include "Sound/SoundBuffer.h"

WASAPISpatialSoundObject::WASAPISpatialSoundObject(int samplingRate, point ptOrigin, vector vEmitterDirection, vector vListenerDirection, ISpatialAudioClient *pAudioSpatialClient, ISpatialAudioObjectRenderStreamForHrtf* pSpatialAudioStreamForHrtf) :
	SpatialSoundObject(samplingRate, ptOrigin, vEmitterDirection, vListenerDirection),
	m_pAudioSpatialClient(pAudioSpatialClient),
	m_pSpatialAudioStreamForHrtf(pSpatialAudioStreamForHrtf)
{
	// empty
}

WASAPISpatialSoundObject::~WASAPISpatialSoundObject() {
	Kill();
}

RESULT WASAPISpatialSoundObject::Kill() {
	RESULT r = R_PASS;

	if (m_pSpatialAudioObjectHRTF != nullptr) {
		m_pSpatialAudioObjectHRTF->SetEndOfStream(0);
		m_pSpatialAudioObjectHRTF = nullptr;
	}

//Error:
	return r;
}

RESULT WASAPISpatialSoundObject::Initialize() {
	RESULT r = R_PASS;

	// Activate a new dynamic audio object

	CNM(m_pAudioSpatialClient, "Spatial audio client invalid");
	CNM(m_pSpatialAudioStreamForHrtf, "Spatial hrtf audio stream invalid");
	
	CRM((RESULT)m_pSpatialAudioStreamForHrtf->ActivateSpatialAudioObjectForHrtf(AudioObjectType::AudioObjectType_Dynamic, &m_pSpatialAudioObjectHRTF),
		"Failed to activate spatial hrtf audio stream");

	CNM(m_pSpatialAudioObjectHRTF, "Failed to allocate spatial hrtf audio object");

	CRM(InitializeSoundBuffer(1, m_samplingRate, sound::type::FLOATING_POINT_32_BIT),
		"Failed to initialize sound buffer for spatial audio object");

Error:
	return r;
}

RESULT WASAPISpatialSoundObject::GetBuffer(BYTE **ppBuffer, UINT32 *pBufferLength) {
	RESULT r = R_PASS;

	CRM((RESULT)m_pSpatialAudioObjectHRTF->GetBuffer(ppBuffer, pBufferLength), "Failed to get buffer");

Error:
	return r;
}

RESULT WASAPISpatialSoundObject::Update(unsigned int numFrames, unsigned int numChannels) {
	RESULT r = R_PASS;

	point ptPosition = GetPosition(true);
	CR((RESULT)m_pSpatialAudioObjectHRTF->SetPosition(ptPosition.x(), ptPosition.y(), ptPosition.z()));

	// TODO: Get from virtual object and camera etc
	CR(UpdateSpatialSoundObjectOrientation());

	// Copy data into buffer
	CR(LoadDataFromBuffer(numFrames, numChannels));

Error:
	return r;
}

RESULT WASAPISpatialSoundObject::LoadDataFromBuffer(unsigned int numFrames, unsigned int numChannels) {
	RESULT r = R_PASS;

	BYTE *pBuffer = nullptr;
	UINT32 pBuffer_n = 0;

	CN(m_pSoundBuffer);

	CR(GetBuffer(&pBuffer, &pBuffer_n));

	float *pDataBuffer = reinterpret_cast<float*>(pBuffer);
	
	size_t numFramesInBuffer = m_pSoundBuffer->NumPendingFrames();
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



RESULT WASAPISpatialSoundObject::WriteTestSignalToAudioObjectBuffer(unsigned int numFrames, unsigned int samplingRate, unsigned int numChannels, float frequency) {
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
RESULT WASAPISpatialSoundObject::UpdateSpatialSoundObjectOrientation() {
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