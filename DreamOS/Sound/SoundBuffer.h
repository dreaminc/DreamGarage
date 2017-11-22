#ifndef SOUND_BUFFER_H_
#define SOUND_BUFFER_H_
#pragma once

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundBuffer.h
// Sound Buffer object (always PCM, simply different types

#include "Primitives/CircularBuffer.h"

class SoundBuffer {
public:
	enum class type {
		UNSIGNED_8_BIT,
		SIGNED_16_BIT,
		FLOATING_POINT_32_BIT,
		FLOATING_POINT_64_BIT,
		INVALID
	};

	static const char * TypeString(SoundBuffer::type bufferType);

public:
	SoundBuffer(int numChannels, SoundBuffer::type bufferType);
	~SoundBuffer();

	virtual SoundBuffer::type GetType() = 0;
	virtual RESULT Initialize() = 0;

	static SoundBuffer* Make(int numChannels, SoundBuffer::type bufferType);

protected:
	int m_channels;
	SoundBuffer::type m_bufferType = type::INVALID;
};

// Type specific stuff
template <class CBType>
class SoundBufferTyped : public SoundBuffer {
public:
	SoundBufferTyped(int numChannels) :
		SoundBuffer(numChannels, GetType())
	{
		// empty
	}

public:
	~SoundBufferTyped() {
		if (m_ppCircularBuffers != nullptr) {
			for (int i = 0; i < m_channels; i++) {
				if (m_ppCircularBuffers[i] != nullptr) {
					delete m_ppCircularBuffers[i];
					m_ppCircularBuffers[i] = nullptr;
				}
			}
			
			delete [] m_ppCircularBuffers;
			m_ppCircularBuffers = nullptr;

		}
	}

	virtual RESULT Initialize() override {
		RESULT r = R_PASS;

		// Currently only support two channels
		CB((m_channels >= 1 && m_channels <= 2));
		CB((m_bufferType != SoundBuffer::type::INVALID));

		m_ppCircularBuffers = new CircularBuffer<CBType>*[m_channels];
		CN(m_ppCircularBuffers);

		for (int i = 0; i < m_channels; i++) {
			m_ppCircularBuffers[i] = new CircularBuffer<CBType>();
			CN(m_ppCircularBuffers[i]);
				
			CR(m_ppCircularBuffers[i]->InitializePendingBuffer());
		}

	Error:
		return r;
	}

	// This is sub-typed below
	virtual SoundBuffer::type GetType() override {
		return SoundBuffer::type::INVALID;
	}

private:
	CircularBuffer<CBType> **m_ppCircularBuffers = nullptr;
};

template<>
SoundBuffer::type SoundBufferTyped<uint8_t>::GetType() { 
	return SoundBuffer::type::UNSIGNED_8_BIT; 
}

template<>
SoundBuffer::type SoundBufferTyped<int16_t>::GetType() { 
	return SoundBuffer::type::UNSIGNED_8_BIT; 
}

template<>
SoundBuffer::type SoundBufferTyped<float>::GetType() { 
	return SoundBuffer::type::FLOATING_POINT_32_BIT; 
}

template<>
SoundBuffer::type SoundBufferTyped<double>::GetType() { 
	return SoundBuffer::type::FLOATING_POINT_64_BIT; 
}

template <class CBType>
SoundBuffer *MakeSoundBuffer(int numChannels) {
	RESULT r = R_PASS;

	SoundBufferTyped<CBType> *pSoundBufferReturn = new SoundBufferTyped<CBType>(numChannels);
	CN(pSoundBufferReturn);

	CR(pSoundBufferReturn->Initialize());

	return pSoundBufferReturn;

Error:
	if (pSoundBufferReturn != nullptr) {
		delete pSoundBufferReturn;
		pSoundBufferReturn = nullptr;
	}

	return nullptr;
}



#endif // SOUND_BUFFER_H_