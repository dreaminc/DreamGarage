#ifndef SOUND_BUFFER_H_
#define SOUND_BUFFER_H_
#pragma once

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundBuffer.h
// Sound Buffer object (always PCM, simply different types

#include <mutex>

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

	virtual bool IsFull() = 0;

public:
	// These are stubs to be picked up by the appropriate template implementation
	virtual RESULT PushData(uint8_t *pDataBuffer, int numFrames) { return R_INVALID_PARAM; }
	virtual RESULT PushData(int16_t *pDataBuffer, int numFrames) { return R_INVALID_PARAM; }
	virtual RESULT PushData(float *pDataBuffer, int numFrames) { return R_INVALID_PARAM; }
	virtual RESULT PushData(double *pDataBuffer, int numFrames) { return R_INVALID_PARAM; }

protected:
	int m_channels;
	SoundBuffer::type m_bufferType = type::INVALID;
	std::mutex m_bufferLock;
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

	virtual bool IsFull() override {
		for (int i = 0; i < m_channels; i++) {
			if (m_ppCircularBuffers[i]->IsFull())
				return true;
		}

		return false;
	}

	// This is sub-typed below
	virtual SoundBuffer::type GetType() override {
		return SoundBuffer::type::INVALID;
	}

	RESULT ReadData(CBType **ppDataBuffer, int channels, int bytesToRead, int &framesRead) {
		RESULT r = R_PASS;

		pDataBuffer = nullptr;
		framesRead = 0;

		CB((m_channels == channels));

		// If lock fails it means we're currently writing the buffer so return 
		// null buffer and zero frames read
		CBR(m_bufferLock.try_lock(), R_BUFFER_NOT_READY);

		for (int i = 0; i < m_channels; i++) {
			int bytesRead;
			m_ppCircularBuffers[i]->ReadFromBuffer(ppDataBuffer[i], bytesToRead, bytesRead);
			CN(ppDataBuffer[i]);
			framesRead = bytesRead;
		}

		m_bufferLock.unlock();

	Error:
		return r;
	}

	RESULT PushData(CBType *pDataBuffer, int numFrames) {
		RESULT r = R_PASS;

		CN(m_ppCircularBuffers);

		m_bufferLock.lock();

		// Make sure the buffers have enough space
		for (int i = 0; i < m_channels; i++) {
			CircularBuffer<CBType> *pChannelCircBuf = m_ppCircularBuffers[i];
			CN(pChannelCircBuf);

			CB((pChannelCircBuf->NumAvailableBufferBytes() >= numFrames));
		}

		// This will de-interlace the samples
		int sampleCount = 0;
		for (int i = 0; i < numFrames; i++) {
			for (int j = 0; j < m_channels; j++) {
				m_ppCircularBuffers[i]->WriteToBuffer(pDataBuffer[sampleCount++]);
			}
		}

		m_bufferLock.unlock();

	Error:
		return r;
	}

private:
	CircularBuffer<CBType> **m_ppCircularBuffers = nullptr;
};

// Unsigned 8 bit int
template<>
SoundBuffer::type SoundBufferTyped<uint8_t>::GetType() { ;
	return SoundBuffer::type::UNSIGNED_8_BIT; 
}

// Signed 16 bit int
template<>
SoundBuffer::type SoundBufferTyped<int16_t>::GetType() { 
	return SoundBuffer::type::UNSIGNED_8_BIT; 
}

// 32 bit floating point
template<>
SoundBuffer::type SoundBufferTyped<float>::GetType() { 
	return SoundBuffer::type::FLOATING_POINT_32_BIT; 
}


// 64 bit floating point
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