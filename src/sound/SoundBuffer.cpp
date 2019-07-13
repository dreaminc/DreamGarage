#include "SoundBuffer.h"

#include "AudioPacket.h"

SoundBuffer::SoundBuffer(int numChannels, int samplingRate, sound::type bufferType) :
	m_channels(numChannels),
	m_samplingRate(samplingRate),
	m_bufferType(bufferType)
{
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

RESULT SoundBuffer::MixMonoAudioPacket(const AudioPacket &audioPacket, float usOffset) {
	RESULT r = R_PASS;

	// Just run mono to all of the channels 
	for (int i = 0; i < m_channels; i++) {
		switch (audioPacket.GetSoundType()) {
		case sound::type::UNSIGNED_8_BIT: {
			CR(MixDataToChannel(i, (uint8_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), usOffset));
		} break;

		case sound::type::SIGNED_16_BIT: {
			CR(MixDataToChannel(i, (int16_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), usOffset));
		} break;

		case sound::type::FLOATING_POINT_32_BIT: {
			CR(MixDataToChannel(i, (float*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), usOffset));
		} break;

		case sound::type::FLOATING_POINT_64_BIT: {
			CR(MixDataToChannel(i, (double*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), usOffset));
		} break;
		}
	}

Error:
	return r;
}

RESULT SoundBuffer::MixAudioPacket(const AudioPacket &audioPacket, float usOffset) {
	RESULT r = R_PASS;

	if (audioPacket.GetNumChannels() == 1) {
		return MixMonoAudioPacket(audioPacket, usOffset);
	}

	switch (audioPacket.GetSoundType()) {
	case sound::type::UNSIGNED_8_BIT: {
		CR(MixData((uint8_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate(), usOffset));
	} break;

	case sound::type::SIGNED_16_BIT: {
		CR(MixData((int16_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate(), usOffset));
	} break;

	case sound::type::FLOATING_POINT_32_BIT: {
		CR(MixData((float*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate(), usOffset));
	} break;

	case sound::type::FLOATING_POINT_64_BIT: {
		CR(MixData((double*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate(), usOffset));
	} break;
	}

Error:
	return r;
}

RESULT SoundBuffer::PushMonoAudioPacket(const AudioPacket &audioPacket, bool fClobber) {
	RESULT r = R_PASS;

	if (fClobber) {
		LockBuffer();

		{
			ResetBuffer(0, 0);
		}

		UnlockBuffer();
	}

	// Just run mono to all of the channels 
	for (int i = 0; i < m_channels; i++) {
		switch (audioPacket.GetSoundType()) {
			case sound::type::UNSIGNED_8_BIT: {
				CR(PushDataToChannel(i, (uint8_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate()));
			} break;

			case sound::type::SIGNED_16_BIT: {
				CR(PushDataToChannel(i, (int16_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate()));
			} break;

			case sound::type::FLOATING_POINT_32_BIT: {
				CR(PushDataToChannel(i, (float*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate()));
			} break;

			case sound::type::FLOATING_POINT_64_BIT: {
				CR(PushDataToChannel(i, (double*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate()));
			} break;
		}
	}

Error:
	return r;
}

// The aptly named clobber param will clear the current contents of the buffer
RESULT SoundBuffer::PushAudioPacket(const AudioPacket &audioPacket, bool fClobber) {
	RESULT r = R_PASS;

	if (audioPacket.GetNumChannels() == 1) {
		return PushMonoAudioPacket(audioPacket, fClobber);
	}

	// TODO: handle mono to stereo
	CBM((audioPacket.GetNumChannels() == m_channels), "channel mismatch");

	if (fClobber) {
		LockBuffer();
		
		{
			ResetBuffer(0, 0);
		}
		
		UnlockBuffer();
	}

	switch (audioPacket.GetSoundType()) {
		case sound::type::UNSIGNED_8_BIT: {
			CR(PushData((uint8_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate()));
		} break;

		case sound::type::SIGNED_16_BIT: {
			CR(PushData((int16_t*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate()));
		} break;

		case sound::type::FLOATING_POINT_32_BIT: {
			CR(PushData((float*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate()));
		} break;

		case sound::type::FLOATING_POINT_64_BIT: {
			CR(PushData((double*)(audioPacket.GetDataBuffer()), audioPacket.GetNumFrames(), audioPacket.GetSamplingRate()));
		} break;
	}

Error:
	return r;
}

RESULT SoundBuffer::GetAudioPacket(int numFrames, AudioPacket *pAudioPacket, bool fUpdateSoundBufferPosition, bool fRequirePending, bool fClearOut) {
	RESULT r = R_PASS;

	CN(pAudioPacket);

	void* pDataBuffer = nullptr;
	size_t pDataBuffer_n = 0;

	CR(GetInterlacedAudioDataBuffer(numFrames, pDataBuffer, pDataBuffer_n, fUpdateSoundBufferPosition, fRequirePending, fClearOut));
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