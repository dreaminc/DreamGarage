#include "AudioPacket.h"

#include <cstdlib>

AudioPacket::AudioPacket() :
	m_frames(0),
	m_channels(0),
	m_bitsPerSample(0),
	m_pDataBuffer(nullptr),
	m_pDataBuffer_n(0)
{
	// empty
}

AudioPacket::AudioPacket(int frames, int channels, int bitsPerSample, uint8_t* pDataBuffer) :
	m_frames(frames),
	m_channels(channels),
	m_bitsPerSample(bitsPerSample),
	m_pDataBuffer(pDataBuffer)
{
	m_pDataBuffer_n = frames * channels * bitsPerSample;
}

AudioPacket::AudioPacket(int frames, int channels, int bitsPerSample, int samplingRate, uint8_t* pDataBuffer) :
	m_frames(frames),
	m_channels(channels),
	m_bitsPerSample(bitsPerSample),
	m_samplingRate(samplingRate),
	m_pDataBuffer(pDataBuffer)
{
	m_pDataBuffer_n = frames * channels * bitsPerSample;
}

AudioPacket::AudioPacket(int frames, int channels, int bitsPerSample, int samplingRate, sound::type soundType, uint8_t* pDataBuffer) :
	m_frames(frames),
	m_channels(channels),
	m_bitsPerSample(bitsPerSample),
	m_samplingRate(samplingRate),
	m_soundType(soundType),
	m_pDataBuffer(pDataBuffer)
{
	m_pDataBuffer_n = frames * channels * bitsPerSample;
}

RESULT AudioPacket::DeleteBuffer() {
	if (m_pDataBuffer != nullptr) {
		delete[] m_pDataBuffer;
		m_pDataBuffer = nullptr;
	}

	m_frames = 0;
	m_channels = 0;
	m_bitsPerSample = 0;
	m_pDataBuffer_n = 0;

	return R_PASS;
}

RESULT AudioPacket::GetDataBufferCopy(uint8_t* &pDataBuffer, size_t &pDataBuffer_n) const {
	RESULT r = R_PASS;

	CB((pDataBuffer == nullptr));

	pDataBuffer_n = GetByteSize();
	pDataBuffer = (uint8_t*)malloc(pDataBuffer_n);
	CN(pDataBuffer);

	memcpy(pDataBuffer, m_pDataBuffer, pDataBuffer_n);

Error:
	return r;
}

// This is an extremely narrowly defined function at the moment
// Basically for mixing in a mono audio packet with equal frames to the target
// and only for the same sampling rate and bit per sample
RESULT AudioPacket::MixInMonoAudioPacket(AudioPacket monoAudioPacket) {
	RESULT r = R_PASS;

	CB((monoAudioPacket.GetNumFrames() == GetNumFrames()));
	CB((monoAudioPacket.GetNumChannels() == 1));
	CB((GetNumChannels() == 2));
	CB((monoAudioPacket.GetSamplingRate() == GetSamplingRate()));

	CB((monoAudioPacket.GetSoundType() == sound::type::SIGNED_16_BIT));
	CB((GetSoundType() == sound::type::SIGNED_16_BIT));

	int16_t *pBufferSignedInt16 = (int16_t*)(m_pDataBuffer);
	int16_t *pMonoBufferSignedInt16 = (int16_t*)(monoAudioPacket.GetDataBuffer());

	for (int i = 0; i < monoAudioPacket.GetNumFrames(); i++) {
		pBufferSignedInt16[(i * 2)] += pMonoBufferSignedInt16[i];
		pBufferSignedInt16[(i * 2) + 1] += pMonoBufferSignedInt16[i];
	}

Error:
	return r;
}

// This is an extremely narrowly defined function at the moment
// Basically for mixing in a stereo audio packet with equal samples and sampling rate
// with 16 bit signed data
RESULT AudioPacket::MixInAudioPacket(AudioPacket audioPacket) {
	RESULT r = R_PASS;

	CB((audioPacket.GetNumSamples() == GetNumSamples()));
	CB((audioPacket.GetNumChannels() == GetNumChannels()));
	CB((audioPacket.GetSamplingRate() == GetSamplingRate()));

	CB((audioPacket.GetSoundType() == sound::type::SIGNED_16_BIT));
	CB((GetSoundType() == sound::type::SIGNED_16_BIT));

	int16_t *pTargetBufferSignedInt16 = (int16_t*)(m_pDataBuffer);
	int16_t *pBufferSignedInt16 = (int16_t*)(audioPacket.GetDataBuffer());

	for (int i = 0; i < audioPacket.GetNumSamples(); i++) {
		pTargetBufferSignedInt16[i] += pBufferSignedInt16[i];
	}

Error:
	return r;
}