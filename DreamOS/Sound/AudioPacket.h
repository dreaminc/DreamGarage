#ifndef AUDIO_PACKET_H_
#define AUDIO_PACKET_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/AudioPacket.h
// This object represents an audio packet 

class AudioPacket {
public:
	AudioPacket(int frames, int channels, int bitsPerSample, uint8_t* pDataBuffer);
	~AudioPacket();

private:
	int m_frames;
	int m_channels;
	int m_bitsPerSample;
	uint8_t *m_pDataBuffer;
	size_t m_pDataBuffer_n;
};


#endif // AUDIO_PACKET_H_