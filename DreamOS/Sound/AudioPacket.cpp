#include "AudioPacket.h"

AudioPacket::AudioPacket(int frames, int channels, int bitsPerSample, uint8_t* pDataBuffer) :
	m_frames(frames),
	m_channels(channels),
	m_bitsPerSample(bitsPerSample),
	m_pDataBuffer(pDataBuffer)
{
	m_pDataBuffer_n = frames * channels * bitsPerSample;
}

AudioPacket::~AudioPacket(){
	// empty for now
}

RESULT AudioPacket::DeleteBuffer() {
	if (m_pDataBuffer != nullptr) {
		delete m_pDataBuffer;
		m_pDataBuffer = nullptr;
	}

	return R_PASS;
}