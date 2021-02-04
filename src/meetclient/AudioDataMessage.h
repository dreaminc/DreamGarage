#ifndef AUDIO_DATA_MESSAGE_H_
#define AUDIO_DATA_MESSAGE_H_

#include "core/ehm/EHM.h"

// Dream Garage
// dos/src/garage/AudioDataMessage.h

#include <string>

#include "DreamGarageMessage.h"

// Likely should be at the DreamOS level of message
class AudioDataMessage : public DreamGarageMessage {
private:
	__declspec(align(8)) struct MessageBody {
		const void* pAudioDataBuffer;
		int bitsPerSample;
		int samplingRate;
		size_t numChannels;
		size_t numFrames;
	} m_body;

public:
	AudioDataMessage(long senderUserID, 
		long receiverUserID, 
		const void* pAudioDataBuffer,
		int bitsPerSample,
		int samplingRate,
		size_t channels,
		size_t frames) :
		DreamGarageMessage(senderUserID, receiverUserID, DreamGarageMessage::type::AUDIO_DATA, sizeof(AudioDataMessage))
	{
		m_body.pAudioDataBuffer = pAudioDataBuffer;
		m_body.bitsPerSample = bitsPerSample;
		m_body.samplingRate = samplingRate;
		m_body.numChannels = channels;
		m_body.numFrames = frames;
	}

	~AudioDataMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	const MessageBody& GetAudioMessageBody() {
		return m_body;
	}

	const void *GetAudioMessageBuffer() {
		return m_body.pAudioDataBuffer;
	}

	int GetBitsPerSample() {
		return m_body.bitsPerSample;
	}

	int GetSamplingRate() {
		return m_body.samplingRate;
	}

	size_t GetNumChannels() {
		return m_body.numChannels;
	}

	size_t GetNumFrames() {
		return m_body.numFrames;
	}
};

#endif	// ! AUDIO_DATA_MESSAGE_H_