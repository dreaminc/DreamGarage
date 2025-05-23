#ifndef AUDIO_PACKET_H_
#define AUDIO_PACKET_H_

#include "core/ehm/EHM.h"

// Dream Sound
// dos/src/sound/AudioPacket.h

// This object represents an audio packet 

#include "SoundCommon.h"

class AudioPacket {
public:
	AudioPacket();
	AudioPacket(int frames, int channels, int bitsPerSample, uint8_t* pDataBuffer);
	AudioPacket(int frames, int channels, int bitsPerSample, int samplingRate, uint8_t* pDataBuffer);
	AudioPacket(int frames, int channels, int bitsPerSample, int samplingRate, sound::type soundType, uint8_t* pDataBuffer);
	
	~AudioPacket() = default;

	uint8_t *GetDataBuffer() const {
		return m_pDataBuffer;
	}

	size_t GetDataBufferSize() const {
		return m_pDataBuffer_n;
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

	size_t GetByteSize() const {
		return (size_t)((m_bitsPerSample / 8) * m_frames * m_channels);
	}

	RESULT SetNumFrames(int numFrames) {
		m_frames = numFrames;
		return R_PASS;
	}

	RESULT SetNumChannels(int numChannels) {
		m_channels = numChannels;
		return R_PASS;
	}

	RESULT SetBitsPerSample(int bitsPerSample) {
		m_bitsPerSample = bitsPerSample;
		return R_PASS;
	}

	RESULT SetSamplingRate(int samplingRate) {
		m_samplingRate = samplingRate;
		return R_PASS;
	}

	RESULT SetDataBuffer(uint8_t *pDatabuffer, size_t pDataBuffer_n) {
		RESULT r = R_PASS;

		CB((m_pDataBuffer == nullptr));

		m_pDataBuffer = pDatabuffer;
		m_pDataBuffer_n = pDataBuffer_n;

	Error:
		return r;
	}

	RESULT SetAudioStreamID(int id) {
		m_audioStreamID = id;
		return R_PASS;
	}

	int GetAudioStreamID() const {
		return m_audioStreamID;
	}

	sound::type GetSoundType() const {
		return m_soundType;
	}

	RESULT SetSoundType(sound::type soundType) {
		m_soundType = soundType;
		return R_PASS;
	}

	RESULT GetDataBufferCopy(uint8_t*&pDataBuffer, size_t &pDataBuffer_n) const;
	RESULT DeleteBuffer();

	RESULT MixInAudioPacket(AudioPacket audioPacket);
	RESULT MixInMonoAudioPacket(AudioPacket monoAudioPacket);

private:
	int m_frames;
	int m_channels;
	int m_bitsPerSample;
	uint8_t *m_pDataBuffer;
	size_t m_pDataBuffer_n;
	sound::type m_soundType = sound::type::INVALID;
	int m_audioStreamID = -1;

	int m_samplingRate = DEFAULT_SAMPLING_RATE;
};


#endif // AUDIO_PACKET_H_