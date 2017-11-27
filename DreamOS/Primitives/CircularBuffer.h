#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/CircularBuffer.h
// A template based circular buffer implementation

// TODO: dynamic style
#define MAX_PENDING_BUFFER_LENGTH (44100 * 2)

template <class CBType>
class CircularBuffer {

private:
	inline void WriteNextValue(CBType value) {
		m_circularBuffer[m_circularBuffer_e] = value;
		m_circularBuffer_e += 1;
		m_numPendingBytes++;

		if (m_circularBuffer_e >= m_circularBuffer_n) {
			m_circularBuffer_e = 0;
		}
	}

public:
	inline RESULT ReadNextValue(CBType &retVal) {
		retVal = 0;
		
		if (m_circularBuffer_c == m_circularBuffer_e) {
			return R_BUFFER_EMPTY;
		}
			
		retVal = m_circularBuffer[m_circularBuffer_c];

		m_circularBuffer_c++;
		m_numPendingBytes--;

		// Circle up
		if (m_circularBuffer_c >= m_circularBuffer_n) {
			m_circularBuffer_c = 0;
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
		memset(&m_circularBuffer, 0, sizeof(CBType) * m_circularBuffer_n);

		m_circularBuffer_e = 0;
		m_circularBuffer_c = 0;
		m_numPendingBytes = 0;

		return R_PASS;
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
		return (m_circularBuffer_n - m_numPendingBytes);
	}

	
	size_t NumPendingBufferBytes() {
		return m_numPendingBytes;
	}

	bool IsPendingBufferEmpty() {
		if (m_numPendingBytes == 0)
			return true;
		else
			return false;
	}

	bool IsFull() {
		if (m_numPendingBytes >= m_circularBuffer_n)
			return true;
		else
			return false;
	}

	RESULT ReadFromBuffer(CBType* &pDataBuffer, size_t bytesToRead, size_t &bytesRead) {
		RESULT r = R_PASS;

		size_t bytesAvailable = NumPendingBufferBytes();

		CBR((bytesAvailable > 0), R_BUFFER_EMPTY);

		if (bytesAvailable < bytesToRead) {
			bytesRead = bytesAvailable;
		}
		else {
			bytesRead = bytesToRead;
		}

		pDataBuffer = (CBType*)(m_circularBuffer + m_circularBuffer_c);

		// Increment current by buffer size
		m_circularBuffer_c += bytesRead;

		// If current overflow overflow, move back to beginning of buffer
		if (m_circularBuffer_c > m_circularBuffer_n) {
			m_circularBuffer_c -= m_circularBuffer_n;
		}

		m_numPendingBytes -= bytesRead;
		CBM((m_numPendingBytes >= 0), "ERROR: CIRCULAR BUFFER PENDING BYTES ERROR");

	Error:
		return r;
	}

private:
	CBType m_circularBuffer[MAX_PENDING_BUFFER_LENGTH];

	size_t m_circularBuffer_n = MAX_PENDING_BUFFER_LENGTH;
	size_t m_circularBuffer_e = 0;
	size_t m_circularBuffer_c = 0;
	size_t m_numPendingBytes = 0;
};

#endif // CIRCULAR_BUFFER_H_