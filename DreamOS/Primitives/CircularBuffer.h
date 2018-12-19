#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/CircularBuffer.h
// A template based circular buffer implementation

// TODO: dynamic style
#define MAX_PENDING_BUFFER_LENGTH (44100 * 20)

struct CircularBufferState {
	size_t m_circularBuffer_n = MAX_PENDING_BUFFER_LENGTH;
	size_t m_circularBuffer_e = 0;
	size_t m_circularBuffer_c = 0;
	size_t m_numPendingBufferSamples = 0;
};

template <class CBType>
class CircularBuffer {

private:
	inline void WriteNextValue(CBType value) {
		m_circularBuffer[m_state.m_circularBuffer_e] = value;
		m_state.m_circularBuffer_e += 1;
		m_state.m_numPendingBufferSamples++;

		if (m_state.m_circularBuffer_e >= m_state.m_circularBuffer_n) {
			m_state.m_circularBuffer_e = 0;
		}
	}

public:
	inline RESULT ReadNextValue(CBType &retVal) {
		retVal = 0;
		
		if (m_state.m_circularBuffer_c == m_state.m_circularBuffer_e) {
			return R_BUFFER_EMPTY;
		}
			
		retVal = m_circularBuffer[m_state.m_circularBuffer_c];

		m_state.m_circularBuffer_c++;
		m_state.m_numPendingBufferSamples--;

		// Circle up
		if (m_state.m_circularBuffer_c >= m_state.m_circularBuffer_n) {
			m_state.m_circularBuffer_c = 0;
		}

		return R_PASS;
	}

public:
	CircularBuffer() {
		// InitializePendingBuffer();
	}

	~CircularBuffer() {
		// empty
	}

	RESULT InitializePendingBuffer() {
		RESULT r = R_PASS;

		CR(ResetPendingBuffer());

	Error:
		return r;
	}

	RESULT ResetPendingBuffer() {
		memset(&m_circularBuffer, 0, sizeof(CBType) * m_state.m_circularBuffer_n);

		m_state.m_circularBuffer_e = 0;
		m_state.m_circularBuffer_c = 0;
		m_state.m_numPendingBufferSamples = 0;

		return R_PASS;
	}

	// This function will not reset the buffer, but allow to 
	// manipulate the buffer (such as would be used to loop audio)
	// This will use the existing data in the buffer
	RESULT SetBufferToValues(size_t startPosition, size_t numPendingFrames) {
		m_state.m_circularBuffer_e = startPosition + numPendingFrames;
		m_state.m_circularBuffer_c = startPosition;
		m_state.m_numPendingBufferSamples = numPendingFrames;

		return R_PASS;
	}

	inline RESULT IncrementBuffer(int count) {
		RESULT r = R_PASS;

		CB((NumPendingBufferSamples() >= count));

		// Increment current by buffer size
		m_state.m_circularBuffer_c += count;

		// If current overflow overflow, move back to beginning of buffer
		if (m_state.m_circularBuffer_c > m_state.m_circularBuffer_n) {
			m_state.m_circularBuffer_c -= m_state.m_circularBuffer_n;
		}

		m_state.m_numPendingBufferSamples -= count;
		CBM((m_state.m_numPendingBufferSamples >= 0), "ERROR: CIRCULAR BUFFER PENDING BYTES ERROR");

	Error:
		return r;
	}

	RESULT WriteToBuffer(CBType *pDataBuffer, size_t pDataBuffer_n) {
		RESULT r = R_PASS;

		CBR((NumAvailableBufferBytes() > 0), R_BUFFER_FULL);

		for (size_t byteCount = 0; byteCount < pDataBuffer_n; byteCount++) {
			WriteNextValue(pDataBuffer[byteCount]);
		}

	Error:
		return r;
	}

	RESULT WriteToBuffer(CBType value) {
		RESULT r = R_PASS;

		CBR((NumAvailableBufferBytes() > 0), R_BUFFER_FULL);

		WriteNextValue(value);

	Error:
		return r;
	}

	size_t NumAvailableBufferBytes() {
		return (m_state.m_circularBuffer_n - m_state.m_numPendingBufferSamples);
	}

	size_t SizeOfCircularBuffer() {
		return m_state.m_circularBuffer_n;
	}
	
	size_t NumPendingBufferSamples() {
		return m_state.m_numPendingBufferSamples;
	}

	bool IsPendingBufferEmpty() {
		if (m_state.m_numPendingBufferSamples == 0)
			return true;
		else
			return false;
	}

	bool IsFull() {
		if (m_state.m_numPendingBufferSamples >= m_state.m_circularBuffer_n)
			return true;
		else
			return false;
	}

	RESULT ReadFromBuffer(CBType* &pDataBuffer, size_t bytesToRead, size_t &bytesRead) {
		RESULT r = R_PASS;

		size_t bytesAvailable = NumPendingBufferSamples();

		CBR((bytesAvailable > 0), R_BUFFER_EMPTY);

		if (bytesAvailable < bytesToRead) {
			bytesRead = bytesAvailable;
		}
		else {
			bytesRead = bytesToRead;
		}

		pDataBuffer = (CBType*)(m_circularBuffer + m_state.m_circularBuffer_c);

		/*
		// Increment current by buffer size
		m_circularBuffer_c += bytesRead;

		// If current overflow overflow, move back to beginning of buffer
		if (m_circularBuffer_c > m_circularBuffer_n) {
			m_circularBuffer_c -= m_circularBuffer_n;
		}

		m_numPendingBytes -= bytesRead;
		CBM((m_numPendingBytes >= 0), "ERROR: CIRCULAR BUFFER PENDING BYTES ERROR");
		*/

		CR(IncrementBuffer(bytesRead));

	Error:
		return r;
	}

	/*
public:
	// Cross conversion

	// Unsigned byte

	template<>
	template<>
	RESULT CircularBuffer<uint8_t>::ReadNextValueTargetType<float>(float &retVal) {
		RESULT r = R_PASS;

		uint8_t nextValue = 0;
		CR(ReadNextValue(nextValue));

		// Apply range
		retVal = (float)((float)nextValue / (float)std::numeric_limits<int8_t>::max());

		// Re-bias the value
		retVal = (retVal * 2.0f) - 1.0f;

	Error:
		return r;
	}

	template<>
	template<>
	RESULT CircularBuffer<uint8_t>::ReadNextValueTargetType<double>(double &retVal) {
		RESULT r = R_PASS;

		uint8_t nextValue = 0;
		CR(ReadNextValue(nextValue));

		// Apply range
		retVal = (double)((double)nextValue / (double)std::numeric_limits<int8_t>::max());

		// Re-bias the value
		retVal = (retVal * 2.0f) - 1.0f;

	Error:
		return r;
	}

	// Signed short

	template<>
	template<>
	RESULT CircularBuffer<int16_t>::ReadNextValueTargetType<float>(float &retVal) {
		RESULT r = R_PASS;

		int16_t nextValue = 0;
		CR(ReadNextValue(nextValue));
		retVal = (float)((float)nextValue / (float)std::numeric_limits<int16_t>::max());

	Error:
		return r;
	}

	template<>
	template<>
	RESULT CircularBuffer<int16_t>::ReadNextValueTargetType<double>(double &retVal) {
		RESULT r = R_PASS;

		int16_t nextValue = 0;
		CR(ReadNextValue(nextValue));
		retVal = (double)((double)nextValue / (double)std::numeric_limits<int16_t>::max());

	Error:
		return r;
	}

	// Float

	template<>
	template<>
	RESULT CircularBuffer<float>::ReadNextValueTargetType<uint8_t>(uint8_t &retVal) {
		RESULT r = R_PASS;

		float nextValue = 0;
		CR(ReadNextValue(nextValue));

		// Set to range of [0.0f, 1.0f] and multiple by the range
		retVal = (uint8_t)(((nextValue + 1.0f) / 2.0f) * std::numeric_limits<uint8_t>::max());

	Error:
		return r;
	}

	template<>
	template<>
	RESULT CircularBuffer<float>::ReadNextValueTargetType<int16_t>(int16_t &retVal) {
		RESULT r = R_PASS;

		float nextValue = 0;
		CR(ReadNextValue(nextValue));
		retVal = (int16_t)(nextValue * std::numeric_limits<int16_t>::max());

	Error:
		return r;
	}

	// Double

	template<>
	template<>
	RESULT CircularBuffer<double>::ReadNextValueTargetType<uint8_t>(uint8_t &retVal) {
		RESULT r = R_PASS;

		double nextValue = 0;
		CR(ReadNextValue(nextValue));

		// Set to range of [0.0f, 1.0f] and multiple by the range
		retVal = (uint8_t)(((nextValue + 1.0f) / 2.0f) * std::numeric_limits<uint8_t>::max());

	Error:
		return r;
	}

	template<>
	template<>
	RESULT CircularBuffer<double>::ReadNextValueTargetType<int16_t>(int16_t &retVal) {
		RESULT r = R_PASS;

		double nextValue = 0;
		CR(ReadNextValue(nextValue));
		retVal = (int16_t)(nextValue * std::numeric_limits<int16_t>::max());

	Error:
		return r;
	}*/

private:
	CBType m_circularBuffer[MAX_PENDING_BUFFER_LENGTH];
	CircularBufferState m_state;

public:
	CircularBufferState GetCircularBufferState() {
		return m_state;
	}

	RESULT SetCircularBufferState(CircularBufferState circularBufferstate) {
		m_state = circularBufferstate;
		return R_PASS;
	}
};




#endif // CIRCULAR_BUFFER_H_