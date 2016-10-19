#ifndef AUDIO_DATA_MESSAGE_H_
#define AUDIO_DATA_MESSAGE_H_

#include "RESULT/EHM.h"


#include <string>
#include "Message.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"
#include "Primitives/quaternion.h"
#include "Primitives/hand.h"

class AudioDataMessage : public Message {
private:
	struct MessageBody {
		const void* audio_data;
		int bits_per_sample;
		int sample_rate;
		size_t number_of_channels;
		size_t number_of_frames;
	} m_body;

public:
	AudioDataMessage(long senderUserID, 
		long receiverUserID, 
		const void* audio_data,
		int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames) :
		Message(senderUserID, receiverUserID, MessageType::MESSAGE_AUDIO_DATA, sizeof(AudioDataMessage))
	{
		m_body.audio_data = audio_data;
		m_body.bits_per_sample = bits_per_sample;
		m_body.sample_rate = sample_rate;
		m_body.number_of_channels = number_of_channels;
		m_body.number_of_frames = number_of_frames;
	}

	~AudioDataMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		return R_NOT_IMPLEMENTED;
	}


	const MessageBody& GetAudioData() {
		return m_body;
	}
};

#endif	// ! AUDIO_DATA_MESSAGE_H_