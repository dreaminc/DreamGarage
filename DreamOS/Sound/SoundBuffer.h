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

protected:
	SoundBuffer(int numChannels, SoundBuffer::type bufferType);
	~SoundBuffer();

public:
	virtual SoundBuffer::type GetType() const = 0;
	virtual RESULT Initialize() = 0;

	static SoundBuffer* Make(int numChannels, SoundBuffer::type bufferType);

	virtual bool IsFull() = 0;
	virtual size_t NumPendingBytes() = 0;

	int NumChannels() {
		return m_channels;
	}

	// This will block
	RESULT LockBuffer() {
		m_bufferLock.lock();
		return R_PASS;
	}

	// This will not block
	bool PeekAndLockBuffer() {
		return m_bufferLock.try_lock();
	}

	RESULT UnlockBuffer() {
		m_bufferLock.unlock();
		return R_PASS;
	}
	
	virtual RESULT PushMonoAudioBuffer(int numFrames, SoundBuffer *pSourceBuffer) = 0;
	virtual RESULT IncrementBuffer(int numFrames) = 0;

public:
	virtual RESULT LoadDataToInterlacedTargetBuffer(uint8_t *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT LoadDataToInterlacedTargetBuffer(int16_t *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT LoadDataToInterlacedTargetBuffer(float *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT LoadDataToInterlacedTargetBuffer(double *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }

public:
	// These are stubs to be picked up by the appropriate template implementation
	virtual RESULT PushData(uint8_t *pDataBuffer, int numFrames) { return R_INVALID_PARAM; }
	virtual RESULT PushData(int16_t *pDataBuffer, int numFrames) { return R_INVALID_PARAM; }
	virtual RESULT PushData(float *pDataBuffer, int numFrames) { return R_INVALID_PARAM; }
	virtual RESULT PushData(double *pDataBuffer, int numFrames) { return R_INVALID_PARAM; }

public:
	virtual RESULT PushDataToChannel(int channel, uint8_t *pDataBuffer, size_t numFrames) { return R_INVALID_PARAM; }
	virtual RESULT PushDataToChannel(int channel, int16_t *pDataBuffer, size_t numFrames) { return R_INVALID_PARAM; }
	virtual RESULT PushDataToChannel(int channel, float *pDataBuffer, size_t numFrames) { return R_INVALID_PARAM; }
	virtual RESULT PushDataToChannel(int channel, double *pDataBuffer, size_t numFrames) { return R_INVALID_PARAM; }

public:
	inline virtual RESULT ReadNextValue(int channel, uint8_t &value) { return R_INVALID_PARAM; }
	inline virtual RESULT ReadNextValue(int channel, int16_t &value) { return R_INVALID_PARAM; }
	inline virtual RESULT ReadNextValue(int channel, float &value) { return R_INVALID_PARAM; }
	inline virtual RESULT ReadNextValue(int channel, double &value) { return R_INVALID_PARAM; }

protected:
	int m_channels;
	SoundBuffer::type m_bufferType = type::INVALID;
	//std::mutex m_bufferLock;
	std::recursive_mutex m_bufferLock;
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

	virtual size_t NumPendingBytes() override {
		size_t numPendingBytes = -1;
		bool fFirst = true;

		// TODO: Right now a lot of work is going into maintaining 
		// the two channel buffers as separate entities and it might 
		// make more sense to just have ONE circular buffer and simply 
		// have SoundBuffer manage the frame structure 

		for (int i = 0; i < m_channels; i++) {
			if (fFirst) {
				numPendingBytes = m_ppCircularBuffers[i]->NumPendingBufferBytes();
				fFirst = false;
			}
			else {
				if (numPendingBytes != m_ppCircularBuffers[i]->NumPendingBufferBytes()) {
					return -1;
				}
			}
		}

		return numPendingBytes;
	}

	virtual RESULT LoadDataToInterlacedTargetBuffer(CBType *pTargetDataBuffer, int numFrameCount) override {
		return R_NOT_IMPLEMENTED;
	}

	// This is sub-typed below
	virtual SoundBuffer::type GetType() const override {
		return SoundBuffer::type::INVALID;
	}

	inline virtual RESULT ReadNextValue(int channel, CBType &value) { 
		return m_ppCircularBuffers[channel]->ReadNextValue(value);
	}

	RESULT IncrementBuffer(int numFrames) {
		RESULT r = R_PASS;

		// This will block
		m_bufferLock.lock();

		for (int i = 0; i < m_channels; i++) {
			CR(m_ppCircularBuffers[i]->IncrementBuffer(numFrames));
		}

		m_bufferLock.unlock();

	Error:
		return r;
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

	RESULT PushMonoAudioBuffer(int numFrames, SoundBuffer *pSourceBuffer) {
		RESULT r = R_PASS;

		CBM((pSourceBuffer->GetType() == GetType()), "Don't currently support different type buffer pushes");
		CB((pSourceBuffer->NumChannels() == 1));

		for (int j = 0; j < numFrames; j++) {
			CBType value = 0;
			pSourceBuffer->ReadNextValue(0, value);

			for (int i = 0; i < m_channels; i++) {
				m_ppCircularBuffers[i]->WriteToBuffer(value);
			}
		}

	Error:
		return r;
	}

	// Pushes interlaced data
	RESULT PushData(CBType *pDataBuffer, int numFrames) {
		RESULT r = R_PASS;

		CN(m_ppCircularBuffers);

		// This will block
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
				m_ppCircularBuffers[j]->WriteToBuffer(pDataBuffer[sampleCount++]);
			}
		}

		m_bufferLock.unlock();

	Error:
		return r;
	}

	// Pushes de-interlaced data to a specific channel
	virtual RESULT PushDataToChannel(int channel, CBType *pDataBuffer, size_t pDataBuffer_n) override {
		RESULT r = R_PASS;

		CN(m_ppCircularBuffers);
		CBM((channel < m_channels), "Channel %d does not exist in SoundBuffer of width %d", channel, m_channels);

		// This will block
		m_bufferLock.lock();

		// Make sure the buffer has enough space
		CircularBuffer<CBType> *pChannelCircBuf = m_ppCircularBuffers[channel];
		CN(pChannelCircBuf);

		CB((pChannelCircBuf->NumAvailableBufferBytes() >= pDataBuffer_n));
	
		pChannelCircBuf->WriteToBuffer(pDataBuffer, pDataBuffer_n);

		m_bufferLock.unlock();

	Error:
		return r;
	}

private:
	CircularBuffer<CBType> **m_ppCircularBuffers = nullptr;
};

// Unsigned 8 bit int
template<>
SoundBuffer::type SoundBufferTyped<uint8_t>::GetType() const { ;
	return SoundBuffer::type::UNSIGNED_8_BIT; 
}

// Signed 16 bit int
template<>
SoundBuffer::type SoundBufferTyped<int16_t>::GetType() const { 
	return SoundBuffer::type::UNSIGNED_8_BIT; 
}

// 32 bit floating point
template<>
SoundBuffer::type SoundBufferTyped<float>::GetType() const { 
	return SoundBuffer::type::FLOATING_POINT_32_BIT; 
}

template<>
RESULT SoundBufferTyped<float>::LoadDataToInterlacedTargetBuffer(float *pTargetDataBuffer, int numFrameCount) {
	RESULT r = R_PASS;

	CB((NumPendingBytes() >= numFrameCount));

	size_t bufferCounter = 0;
	float tempVal = 0.0f;

	for (int j = 0; j < numFrameCount; j++) {
		for (int i = 0; i < m_channels; i++) {
			CR(m_ppCircularBuffers[i]->ReadNextValue(tempVal));

			pTargetDataBuffer[bufferCounter] = tempVal;
			bufferCounter++;
		}
	}

Error:
	return r;
}

// 64 bit floating point
template<>
SoundBuffer::type SoundBufferTyped<double>::GetType() const { 
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