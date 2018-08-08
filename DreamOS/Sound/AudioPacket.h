#ifndef AUDIO_PACKET_H_
#define AUDIO_PACKET_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/AudioPacket.h
// This object represents an audio packet 

#define DEFAULT_SAMPLING_RATE 44100

class AudioPacket {
public:
	AudioPacket(int frames, int channels, int bitsPerSample, uint8_t* pDataBuffer);
	AudioPacket(int frames, int channels, int bitsPerSample, int samplingRate, uint8_t* pDataBuffer);
	
	~AudioPacket();

	uint8_t *GetDataBuffer() const {
		return m_pDataBuffer;
	}

	int GetNumFrames() const {
		return m_frames;
	}

	int GetNumSamples() const {
		return m_frames * m_channels;
	}

	int GetNumChannels() const {
		return m_channels;
	}

	int GetBitsPerSample() const {
		return m_bitsPerSample;
	}

	int GetBytesPerSample() const {
		return (m_bitsPerSample / 8);
	}

	int GetSamplingRate() const {
		return m_samplingRate;
	}

	RESULT DeleteBuffer();

private:
	int m_frames;
	int m_channels;
	int m_bitsPerSample;
	uint8_t *m_pDataBuffer;
	size_t m_pDataBuffer_n;

	int m_samplingRate = DEFAULT_SAMPLING_RATE;
};


#endif // AUDIO_PACKET_H_