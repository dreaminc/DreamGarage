#include "SoundBuffer.h"

#include "AudioPacket.h"

SoundBuffer::SoundBuffer(int numChannels, int samplingRate, sound::type bufferType) :
	m_channels(numChannels),
	m_samplingRate(samplingRate),
	m_bufferType(bufferType)
{
	// empty
}

SoundBuffer::~SoundBuffer() {
	// empty
}

SoundBuffer* SoundBuffer::Make(int numChannels, int samplingRate, sound::type bufferType) {
	RESULT r = R_PASS;
	SoundBuffer *pSoundBuffer = nullptr;

	switch (bufferType) {
	case sound::type::UNSIGNED_8_BIT: {
		pSoundBuffer = MakeSoundBuffer<uint8_t>(numChannels, samplingRate);
		CN(pSoundBuffer);
	} break;

	case sound::type::SIGNED_16_BIT: {
		pSoundBuffer = MakeSoundBuffer<int16_t>(numChannels, samplingRate);
		CN(pSoundBuffer);
	} break;

	case sound::type::FLOATING_POINT_32_BIT: {
		pSoundBuffer = MakeSoundBuffer<float>(numChannels, samplingRate);
		CN(pSoundBuffer);
	} break;

	case sound::type::FLOATING_POINT_64_BIT: {
		pSoundBuffer = MakeSoundBuffer<double>(numChannels, samplingRate);
		CN(pSoundBuffer);
	} break;
	}

	return pSoundBuffer;

Error:
	if (pSoundBuffer != nullptr) {
		delete pSoundBuffer;
		pSoundBuffer = nullptr;
	}

	return nullptr;
}

const char* SoundBuffer::TypeString(sound::type bufferType) {
	switch (bufferType) {
	case sound::type::UNSIGNED_8_BIT: return "8 bit unsigned"; break;
	case sound::type::SIGNED_16_BIT: return "16 bit signed"; break;
	case sound::type::FLOATING_POINT_32_BIT: return "32 bit floating point"; break;
	case sound::type::FLOATING_POINT_64_BIT: return "64 bit floating point"; break;
	}

	return "invalid";
}

RESULT SoundBuffer::PushAudioPacket(const AudioPacket &audioPacket) {
	RESULT r = R_PASS;

	CBM((audioPacket.GetNumChannels() == m_channels), "Channel mismatch");

	switch (audioPacket.GetSoundType()) {
		case sound::type::UNSIGNED_8_BIT: {
			CR(PushData((uint8_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames()));
		} break;

		case sound::type::SIGNED_16_BIT: {
			CR(PushData((int16_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames()));
		} break;

		case sound::type::FLOATING_POINT_32_BIT: {
			CR(PushData((float*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames()));
		} break;

		case sound::type::FLOATING_POINT_64_BIT: {
			CR(PushData((double*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames()));
		} break;
	}

Error:
	return r;
}

RESULT SoundBuffer::GetAudioPacket(int numFrames, AudioPacket *pAudioPacket) {
	RESULT r = R_PASS;

	CN(pAudioPacket);

	void* pDataBuffer = nullptr;
	size_t pDataBuffer_n = 0;

	CR(GetInterlacedAudioDataBuffer(numFrames, pDataBuffer, pDataBuffer_n));
	CN(pDataBuffer);

	// Set it
	pAudioPacket->SetNumFrames(numFrames);
	pAudioPacket->SetNumChannels(NumChannels());
	pAudioPacket->SetBitsPerSample(GetBitsPerSample());
	pAudioPacket->SetSamplingRate(GetSamplingRate());
	pAudioPacket->SetSoundType(m_bufferType);
	pAudioPacket->SetDataBuffer((uint8_t*)pDataBuffer, pDataBuffer_n);

Error:
	return r;
}