#ifndef SOUND_BUFFER_H_
#define SOUND_BUFFER_H_
#pragma once

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundBuffer.h
// Sound Buffer object (always PCM, simply different types

#include <mutex>

#include "SoundCommon.h"
#include "Primitives/CircularBuffer.h"

class AudioPacket;

class SoundBuffer {
public:
	static const char *TypeString(sound::type bufferType);

protected:
	SoundBuffer(int numChannels, int samplingRate, sound::type bufferType);
	~SoundBuffer();

public:
	virtual sound::type GetType() const = 0;
	virtual RESULT Initialize() = 0;

	static SoundBuffer* Make(int numChannels, int samplingRate, sound::type bufferType);

	virtual bool IsFull() = 0;
	virtual int64_t NumPendingFrames(int64_t *optMinFrames = nullptr, int64_t *optMaxFrames = nullptr) = 0;

	int NumChannels() {
		return m_channels;
	}

	int GetSamplingRate() {
		return m_samplingRate;
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
	virtual RESULT IncrementBufferChannel(int channel, int numFrames) = 0;
	virtual RESULT GetAudioPacket(int numFrames, AudioPacket *pAudioPacket);
	virtual RESULT PushAudioPacket(const AudioPacket &audioPacket, bool fClobber = false);

	virtual RESULT ResetBuffer(size_t startPosition, size_t numPendingFrames) = 0;

	virtual int GetBytesPerFrame() { return 0; }
	virtual int GetBitsPerFrame() { return 0; }
	virtual int GetBytesPerSample() { return 0; }
	virtual int GetBitsPerSample() { return 0; }

public:
	virtual RESULT GetInterlacedAudioDataBuffer(int numFrames, void* &n_pDataBuffer, size_t &m_pDataBuffer_n) = 0;
	
public:
	virtual RESULT LoadDataToInterlacedTargetBufferTargetType(uint8_t *pTargetDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT LoadDataToInterlacedTargetBufferTargetType(int16_t *pTargetDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT LoadDataToInterlacedTargetBufferTargetType(float *pTargetDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT LoadDataToInterlacedTargetBufferTargetType(double *pTargetDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }

public:
	virtual RESULT LoadDataToInterlacedTargetBuffer(uint8_t *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT LoadDataToInterlacedTargetBuffer(int16_t *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT LoadDataToInterlacedTargetBuffer(float *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT LoadDataToInterlacedTargetBuffer(double *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }

public:
	virtual RESULT MixIntoInterlacedTargetBuffer(uint8_t *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT MixIntoInterlacedTargetBuffer(int16_t *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT MixIntoInterlacedTargetBuffer(float *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }
	virtual RESULT MixIntoInterlacedTargetBuffer(double *pDataBuffer, int numFrameCount) { return R_INVALID_PARAM; }

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
	int m_samplingRate = DEFAULT_SAMPLING_RATE;

	sound::type m_bufferType = sound::type::INVALID;
	//std::mutex m_bufferLock;
	std::recursive_mutex m_bufferLock;
};

// Type specific stuff
template <class CBType>
class SoundBufferTyped : public SoundBuffer {
public:
	SoundBufferTyped(int numChannels, int samplingRate) :
		SoundBuffer(numChannels, samplingRate, GetType())
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
		CB((m_bufferType != sound::type::INVALID));

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

	// TODO: Is it really bytes?
	virtual int64_t NumPendingFrames(int64_t *optMinFrames = nullptr, int64_t *optMaxFrames = nullptr) override {
		int64_t numPendingBytes = -1;
		bool fFirst = true;

		// TODO: Right now a lot of work is going into maintaining 
		// the two channel buffers as separate entities and it might 
		// make more sense to just have ONE circular buffer and simply 
		// have SoundBuffer manage the frame structure 

		for (int i = 0; i < m_channels; i++) {
			if (fFirst) {
				numPendingBytes = m_ppCircularBuffers[i]->NumPendingBufferSamples();
				fFirst = false;
			}
			else {
				if (numPendingBytes != m_ppCircularBuffers[i]->NumPendingBufferSamples()) {
					return -1;
				}
			}
		}

		return numPendingBytes;
	}

	virtual RESULT ResetBuffer(size_t startPosition, size_t numPendingFrames) override {
		RESULT r = R_PASS;

		for (int i = 0; i < m_channels; i++) {
			CR(m_ppCircularBuffers[i]->SetBufferToValues(startPosition, numPendingFrames));
		}

	Error:
		return r;
	}

	// This is sub-typed below
	virtual sound::type GetType() const override {
		return sound::type::INVALID;
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

	virtual RESULT IncrementBufferChannel(int channel, int numFrames) override {
		RESULT r = R_PASS;

		// This will block
		m_bufferLock.lock();

		CB((channel < m_channels));
		CR(m_ppCircularBuffers[channel]->IncrementBuffer(numFrames));

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

		int sampleCount = 0;

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

		CircularBuffer<CBType> *pChannelCircBuf = nullptr;

		CN(m_ppCircularBuffers);
		CBM((channel < m_channels), "Channel %d does not exist in SoundBuffer of width %d", channel, m_channels);

		// This will block
		m_bufferLock.lock();

		// Make sure the buffer has enough space
		pChannelCircBuf = m_ppCircularBuffers[channel];
		CN(pChannelCircBuf);

		CB((pChannelCircBuf->NumAvailableBufferBytes() >= pDataBuffer_n));
	
		pChannelCircBuf->WriteToBuffer(pDataBuffer, pDataBuffer_n);

		m_bufferLock.unlock();

	Error:
		return r;
	}

	virtual RESULT MixIntoInterlacedTargetBuffer(CBType *pDataBuffer, int numFrameCount) override {
		RESULT r = R_PASS;

		CBR((NumPendingFrames() >= numFrameCount), R_SKIPPED);
		
		{
			size_t bufferCounter = 0;
			CBType tempVal = 0;

			for (int j = 0; j < numFrameCount; j++) {
				for (int i = 0; i < m_channels; i++) {
					CR(m_ppCircularBuffers[i]->ReadNextValue(tempVal));

					pDataBuffer[bufferCounter] += tempVal;
					bufferCounter++;
				}
			}
		}

	Error:
		return r;
	}

	virtual int GetBytesPerFrame() override { 
		return (m_channels * sizeof(CBType));
	}

	virtual int GetBytesPerSample() override {
		return sizeof(CBType);
	}

	virtual int GetBitsPerFrame() override {
		return (m_channels * (sizeof(CBType) << 3));
	}

	virtual int GetBitsPerSample() override {
		return (sizeof(CBType) << 3);
	}

	virtual RESULT GetInterlacedAudioDataBuffer(int numFrames, void* &n_pDataBuffer, size_t &m_pDataBuffer_n) override {
		RESULT r = R_SKIPPED;

		CBType *pTargetDataBuffer = nullptr;
		size_t bufferLength = 0;

		CB((n_pDataBuffer == nullptr));

		bufferLength = numFrames * m_channels;
		pTargetDataBuffer = new CBType[bufferLength];
		CN(pTargetDataBuffer);

		CR(LoadDataToInterlacedTargetBuffer(pTargetDataBuffer, numFrames));
		n_pDataBuffer = (void*)(pTargetDataBuffer);
		m_pDataBuffer_n = bufferLength * sizeof(CBType);

		return r;

	Error:
		if (pTargetDataBuffer != nullptr) {
			delete[] pTargetDataBuffer;
			pTargetDataBuffer = nullptr;
		}

		return r;
	}

	virtual RESULT LoadDataToInterlacedTargetBuffer(CBType *pTargetDataBuffer, int numFrameCount) override {
		RESULT r = R_PASS;

		//if (NumPendingBytes() >= numFrameCount) 
		
		CBR((NumPendingFrames() >= numFrameCount), R_SKIPPED);

		{	
			size_t bufferCounter = 0;
			CBType tempVal = 0;

			for (int j = 0; j < numFrameCount; j++) {
				for (int i = 0; i < m_channels; i++) {

					CRM(m_ppCircularBuffers[i]->ReadNextValue(tempVal), "Read next value failed");

					pTargetDataBuffer[bufferCounter] = tempVal;
					bufferCounter++;

				}
			}
		}

	Error:
		return r;
	}

	// TODO: Fix the templates, there's a lot of duplicated code in these functions 
	// but can't seem to push the templating around to make it work without it

	virtual RESULT LoadDataToInterlacedTargetBufferTargetType(uint8_t *pTargetDataBuffer, int numFrameCount) override {
		RESULT r = R_PASS;
	
		CBR((NumPendingFrames() >= numFrameCount), R_SKIPPED);
	
		{
			size_t bufferCounter = 0;
			CBType tempVal = 0;
	
			for (int j = 0; j < numFrameCount; j++) {
				for (int i = 0; i < m_channels; i++) {
	
					CRM(m_ppCircularBuffers[i]->ReadNextValue(tempVal), "Read next value failed");
					
					switch (m_bufferType) {
						case sound::type::UNSIGNED_8_BIT: {
							pTargetDataBuffer[bufferCounter] = tempVal;
						} break;

						case sound::type::SIGNED_16_BIT: {
							float floatVal = (float)((float)tempVal / (float)std::numeric_limits<int16_t>::max());
							pTargetDataBuffer[bufferCounter] = ((floatVal + 1.0f) / 2.0f) * std::numeric_limits<uint8_t>::max();
						} break;

						case sound::type::FLOATING_POINT_32_BIT: {
							pTargetDataBuffer[bufferCounter] = ((tempVal + 1.0f) / 2.0f) * std::numeric_limits<uint8_t>::max();
						} break;

						case sound::type::FLOATING_POINT_64_BIT: {
							pTargetDataBuffer[bufferCounter] = ((tempVal + 1.0f) / 2.0f) * std::numeric_limits<uint8_t>::max();
						} break;
					}

					bufferCounter++;
				}
			}
		}
	
	Error:
		return r;
	}	

	virtual RESULT LoadDataToInterlacedTargetBufferTargetType(int16_t *pTargetDataBuffer, int numFrameCount) override {
		RESULT r = R_PASS;

		CBR((NumPendingFrames() >= numFrameCount), R_SKIPPED);

		{
			size_t bufferCounter = 0;
			CBType tempVal = 0;

			for (int j = 0; j < numFrameCount; j++) {
				for (int i = 0; i < m_channels; i++) {

					CRM(m_ppCircularBuffers[i]->ReadNextValue(tempVal), "Read next value failed");

					switch (m_bufferType) {
						case sound::type::UNSIGNED_8_BIT: {
							float floatVal = (float)((float)tempVal / (float)std::numeric_limits<uint8_t>::max());
							pTargetDataBuffer[bufferCounter] = ((floatVal * 2.0f) - 1.0f) * std::numeric_limits<int16_t>::max();
						} break;

						case sound::type::SIGNED_16_BIT: {
							pTargetDataBuffer[bufferCounter] = tempVal;
						} break;

						case sound::type::FLOATING_POINT_32_BIT: {
							pTargetDataBuffer[bufferCounter] = (int16_t)(tempVal * std::numeric_limits<int16_t>::max());
						} break;

						case sound::type::FLOATING_POINT_64_BIT: {
							pTargetDataBuffer[bufferCounter] = (int16_t)(tempVal * std::numeric_limits<int16_t>::max());
						} break;
					}

					bufferCounter++;

				}
			}
		}

	Error:
		return r;
	}

	virtual RESULT LoadDataToInterlacedTargetBufferTargetType(float *pTargetDataBuffer, int numFrameCount) override {
		RESULT r = R_PASS;

		CBR((NumPendingFrames() >= numFrameCount), R_SKIPPED);

		{
			size_t bufferCounter = 0;
			CBType tempVal = 0;

			for (int j = 0; j < numFrameCount; j++) {
				for (int i = 0; i < m_channels; i++) {

					CRM(m_ppCircularBuffers[i]->ReadNextValue(tempVal), "Read next value failed");

					switch (m_bufferType) {
						case sound::type::UNSIGNED_8_BIT: {
							float floatVal = (float)((float)tempVal / (float)std::numeric_limits<uint8_t>::max());
							pTargetDataBuffer[bufferCounter] = ((floatVal * 2.0f) - 1.0f);
						} break;

						case sound::type::SIGNED_16_BIT: {
							float floatVal = (float)((float)tempVal / (float)std::numeric_limits<int16_t>::max());
							pTargetDataBuffer[bufferCounter] = floatVal;
							
						} break;

						case sound::type::FLOATING_POINT_32_BIT: {
							pTargetDataBuffer[bufferCounter] = tempVal;
						} break;

						case sound::type::FLOATING_POINT_64_BIT: {
							pTargetDataBuffer[bufferCounter] = (float)tempVal;
						} break;
					}

					bufferCounter++;

				}
			}
		}

	Error:
		return r;
	}

	virtual RESULT LoadDataToInterlacedTargetBufferTargetType(double *pTargetDataBuffer, int numFrameCount) override {
		RESULT r = R_PASS;

		CBR((NumPendingFrames() >= numFrameCount), R_SKIPPED);

		{
			size_t bufferCounter = 0;
			CBType tempVal = 0;

			for (int j = 0; j < numFrameCount; j++) {
				for (int i = 0; i < m_channels; i++) {

					CRM(m_ppCircularBuffers[i]->ReadNextValue(tempVal), "Read next value failed");

					switch (m_bufferType) {
						case sound::type::UNSIGNED_8_BIT: {
							double floatVal = (double)((double)tempVal / (double)std::numeric_limits<uint8_t>::max());
							pTargetDataBuffer[bufferCounter] = ((floatVal * 2.0f) - 1.0f);
						} break;

						case sound::type::SIGNED_16_BIT: {
							double floatVal = (double)((double)tempVal / (double)std::numeric_limits<int16_t>::max());
							pTargetDataBuffer[bufferCounter] = floatVal;

						} break;

						case sound::type::FLOATING_POINT_32_BIT: {
							pTargetDataBuffer[bufferCounter] = (double)tempVal;
						} break;

						case sound::type::FLOATING_POINT_64_BIT: {
							pTargetDataBuffer[bufferCounter] = tempVal;
						} break;
					}

					bufferCounter++;

				}
			}
		}

	Error:
		return r;
	}

private:
	CircularBuffer<CBType> **m_ppCircularBuffers = nullptr;
};

// Unsigned 8 bit int
template<>
sound::type SoundBufferTyped<uint8_t>::GetType() const { ;
	return sound::type::UNSIGNED_8_BIT; 
}

// Signed 16 bit int
template<>
sound::type SoundBufferTyped<int16_t>::GetType() const { 
	return sound::type::SIGNED_16_BIT; 
}

// 32 bit floating point
template<>
sound::type SoundBufferTyped<float>::GetType() const { 
	return sound::type::FLOATING_POINT_32_BIT; 
}

// 64 bit floating point
template<>
sound::type SoundBufferTyped<double>::GetType() const { 
	return sound::type::FLOATING_POINT_64_BIT; 
}

template <class CBType>
SoundBuffer *MakeSoundBuffer(int numChannels, int samplingRate) {
	RESULT r = R_PASS;

	SoundBufferTyped<CBType> *pSoundBufferReturn = new SoundBufferTyped<CBType>(numChannels, samplingRate);
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