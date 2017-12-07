#include "SoundBuffer.h"

SoundBuffer::SoundBuffer(int numChannels, SoundBuffer::type bufferType) :
	m_channels(numChannels),
	m_bufferType(bufferType)
{
	// empty
}

SoundBuffer::~SoundBuffer() {
	// empty
}

SoundBuffer* SoundBuffer::Make(int numChannels, SoundBuffer::type bufferType) {
	RESULT r = R_PASS;
	SoundBuffer *pSoundBuffer = nullptr;

	switch (bufferType) {
	case SoundBuffer::type::UNSIGNED_8_BIT: {
		pSoundBuffer = MakeSoundBuffer<uint8_t>(numChannels);
		CN(pSoundBuffer);
	} break;

	case SoundBuffer::type::SIGNED_16_BIT: {
		pSoundBuffer = MakeSoundBuffer<int16_t>(numChannels);
		CN(pSoundBuffer);
	} break;

	case SoundBuffer::type::FLOATING_POINT_32_BIT: {
		pSoundBuffer = MakeSoundBuffer<float>(numChannels);
		CN(pSoundBuffer);
	} break;

	case SoundBuffer::type::FLOATING_POINT_64_BIT: {
		pSoundBuffer = MakeSoundBuffer<double>(numChannels);
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

const char* SoundBuffer::TypeString(SoundBuffer::type bufferType) {
	switch (bufferType) {
	case type::UNSIGNED_8_BIT: return "8 bit unsigned"; break;
	case type::SIGNED_16_BIT: return "16 bit signed"; break;
	case type::FLOATING_POINT_32_BIT: return "32 bit floating point"; break;
	case type::FLOATING_POINT_64_BIT: return "64 bit floating point"; break;
	}

	return "invalid";
}