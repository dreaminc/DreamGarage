#ifndef SOUND_BUFFER_H_
#define SOUND_BUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundBuffer.h
// Sound Buffer object (always PCM, simply different types

#include "Primitives/CircularBuffer.h"

class SoundBufferBase {
public:
	enum class type {
		UNSIGNED_8_BIT,
		SIGNED_16_BIT,
		FLOATING_POINT_32_BIT,
		FLOATING_POINT_64_BIT,
		INVALID
	};

protected:
	SoundBufferBase(int numChannels, SoundBufferBase::type bufferType) :
		m_channels(numChannels),
		m_bufferType(bufferType)
	{
		// empty
	}

	~SoundBufferBase() {
		// empty
	}

	virtual SoundBufferBase::type GetType() = 0;

public:
	virtual RESULT Initialize() = 0;

protected:
	int m_channels;
	SoundBufferBase::type m_bufferType = type::INVALID;
};

template <class CBType>
class SoundBuffer : public SoundBufferBase {
	SoundBuffer(int numChannels, SoundBufferBase::type bufferType) :
		SoundBuffer(numChannels, SoundBufferBase::type bufferType)
	{
		// empty
	}

	~SoundBuffer() {
		if (m_pCircularBuffers != nullptr) {
			for (int i = 0; i < m_channels; i++) {
				if (m_pCircularBuffers[i] != nullptr) {
					delete m_pCircularBuffers[i];
					m_pCircularBuffers[i] = nullptr;
				}
			}
			
			delete [] m_pCircularBuffers;
			m_pCircularBuffers = nullptr;

		}
	}

	virtual RESULT Initialize() override {
		RESULT r = R_PASS;

		// Currently only support two channels
		CB((m_channels >= 1 && m_channels <= 2));
		CB((m_bufferType != SoundBuffer::type::INVALID));

		m_pCircularBuffers = new CircularBuffer<CBType>[m_channels];
		CN(m_pCircularBuffers);

		for (int i = 0; i < m_channels; i++) {
			m_pCircularBuffers[i].InitializePendingBuffer();
		}

	Error:
		return r;
	}

	// This is sub-typed below
	virtual SoundBufferBase::type GetType() override;

	static SoundBuffer *Make(int numChannels) {
		RESULT r = R_PASS;

		SoundBufferBase::type bufferType = GetType();

		SoundBuffer<CBType> pSoundBufferReturn = new SoundBuffer<CBType>(numChannels, bufferType);
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

private:
	CircularBuffer<CBType> *m_pCircularBuffers = nullptr;
};

template<>
SoundBufferBase::type SoundBuffer<uint8_t>::GetType() { return SoundBufferBase::type::UNSIGNED_8_BIT; }

template<>
SoundBufferBase::type SoundBuffer<int16_t>::GetType() { return SoundBufferBase::type::UNSIGNED_8_BIT; }

template<>
SoundBufferBase::type SoundBuffer<float>::GetType() { return SoundBufferBase::type::FLOATING_POINT_32_BIT; }

template<>
SoundBufferBase::type SoundBuffer<double>::GetType() { return SoundBufferBase::type::FLOATING_POINT_64_BIT; }

#endif // SOUND_BUFFER_H_