#include "SpatialSoundObject.h"

#include "SoundFile.h"

SpatialSoundObject::SpatialSoundObject(point ptOrigin, vector vEmitterDirection, vector vListenerDirection) :
	VirtualObj(ptOrigin),
	m_vEmitterDirection(vEmitterDirection),
	m_vListenerDirection(vListenerDirection)
{
	// empty
}

RESULT SpatialSoundObject::SetEmitterListenerDirection(vector vEmitterDirection, vector vListenerDirection) {
	m_vEmitterDirection = vEmitterDirection;
	m_vListenerDirection = vListenerDirection;

	return R_PASS;
}

RESULT SpatialSoundObject::PushMonoAudioBuffer(int numFrames, const float *pSoundBuffer) {
	RESULT r = R_PASS;

	CN(pSoundBuffer);

	m_pSoundBuffer->LockBuffer();
	{
		if (m_pSoundBuffer->IsFull() == false) {
			m_pSoundBuffer->PushData((float*)(pSoundBuffer), numFrames);
		}
		else {
			DEBUG_LINEOUT("Render buffer is full");
		}
	}
	m_pSoundBuffer->UnlockBuffer();

Error:
	return r;
}

RESULT SpatialSoundObject::PushMonoAudioBuffer(int numFrames, const int16_t *pSoundBuffer) {
	RESULT r = R_PASS;

	CN(pSoundBuffer);

	m_pSoundBuffer->LockBuffer();
	{
		if (m_pSoundBuffer->IsFull() == false) {
			m_pSoundBuffer->PushData((int16_t*)(pSoundBuffer), numFrames);
		}
		else {
			DEBUG_LINEOUT("Render buffer is full");
		}
	}
	m_pSoundBuffer->UnlockBuffer();

Error:
	return r;
}

RESULT SpatialSoundObject::PushMonoAudioBuffer(int numFrames, SoundBuffer *pSoundBuffer) {
	RESULT r = R_PASS;

	float *pFloatAudioBuffer = nullptr;

	CN(pSoundBuffer);

	//CBM((pSoundFile->NumChannels() == m_pSoundBuffer->NumChannels()),
	//	"Don't currently support playing files that don't match render buffer channel count");

	m_pSoundBuffer->LockBuffer();
	{
		if (m_pSoundBuffer->IsFull() == false) {
			m_pSoundBuffer->PushMonoAudioBuffer(numFrames, pSoundBuffer);
		}
		else {
			DEBUG_LINEOUT("Render buffer is full");
		}
	}
	m_pSoundBuffer->UnlockBuffer();

Error:
	return r;
}

RESULT SpatialSoundObject::LoadSoundFile(SoundFile *pSoundFile) {
	RESULT r = R_PASS;
	float *pFloatAudioBuffer = nullptr;

	CN(pSoundFile);

	//CBM((pSoundFile->NumChannels() == m_pSoundBuffer->NumChannels()),
	//	"Don't currently support playing files that don't match render buffer channel count");

	m_pSoundBuffer->LockBuffer();
	{
		if (m_pSoundBuffer->IsFull() == false) {
			
			CR(pSoundFile->GetAudioBuffer(pFloatAudioBuffer, 1));

			CR(m_pSoundBuffer->PushData(pFloatAudioBuffer, pSoundFile->GetNumFrames()));
		}
	}
	m_pSoundBuffer->UnlockBuffer();

Error:
	if (pFloatAudioBuffer != nullptr) {
		delete[] pFloatAudioBuffer;
		pFloatAudioBuffer = nullptr;
	}

	return r;
}

RESULT SpatialSoundObject::PlaySoundFile(SoundFile *pSoundFile) {
	RESULT r = R_PASS;

	m_fLoop = false;

	CR(LoadSoundFile(pSoundFile));

Error:
	return r;
}

RESULT SpatialSoundObject::LoopSoundFile(SoundFile *pSoundFile) {
	RESULT r = R_PASS;

	m_fLoop = true;

	m_startLoop = 0;
	m_endLoop = pSoundFile->GetNumFrames();

	CR(LoadSoundFile(pSoundFile));

Error:
	return r;
}

RESULT SpatialSoundObject::InitializeSoundBuffer(int numChannels, SoundBuffer::type bufferType) {
	RESULT r = R_PASS;

	CB((m_pSoundBuffer == nullptr));

	m_pSoundBuffer = SoundBuffer::Make(numChannels, bufferType);
	CN(m_pSoundBuffer);

	DEBUG_LINEOUT("Initialized Spatial Object Sound Buffer %d channels type: %s", numChannels, SoundBuffer::TypeString(bufferType));

Error:
	return r;
}