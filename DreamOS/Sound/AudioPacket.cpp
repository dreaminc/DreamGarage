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

AudioPacket::~AudioPacket(){
	// empty for now
}

RESULT AudioPacket::DeleteBuffer() {
	if (m_pDataBuffer != nullptr) {
		free(m_pDataBuffer);
		m_pDataBuffer = nullptr;
	}

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