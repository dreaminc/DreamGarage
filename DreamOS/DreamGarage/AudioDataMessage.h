#ifndef AUDIO_DATA_MESSAGE_H_
#define AUDIO_DATA_MESSAGE_H_

#include "RESULT/EHM.h"

#include <string>
#include "DreamGarageMessage.h"

class AudioDataMessage : public DreamGarageMessage {
private:
	__declspec(align(8)) struct MessageBody {
		const void* pAudioDataBuffer;
		int bitsPerSample;
		int samplingRate;
		size_t channels;
		size_t frames;
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
		m_body.channels = channels;
		m_body.frames = frames;
	}

	~AudioDataMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		return R_NOT_IMPLEMENTED;
	}

	const MessageBody& GetAudioMessageBody() {
		return m_body;
	}

	const void *GetAudioMessageBuffer() {
		return m_body.pAudioDataBuffer;
	}

	const int GetBitsPerSample() {
		return m_body.bitsPerSample;
	}

	const int GetSamplingRate() {
		return m_body.samplingRate;
	}

	const size_t GetChannels() {
		return m_body.channels;
	}

	const size_t GetFrames() {
		return m_body.frames;
	}
};

#endif	// ! AUDIO_DATA_MESSAGE_H_