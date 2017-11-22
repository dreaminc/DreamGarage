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

		return R_PASS;
	}

	RESULT WriteToBuffer(CBType *pDataBuffer, size_t pDataBuffer_n) {
		RESULT r = R_PASS;

		CBR((NumAvailableBufferBytes() >= pDataBuffer_n), R_BUFFER_FULL);

		for (size_t byteCount = 0; byteCount < pDataBuffer_n; byteCount++) {
			m_circularBuffer[m_circularBuffer_e] = pDataBuffer[byteCount];
			m_circularBuffer_e += 1;

			if (m_circularBuffer_e >= m_circularBuffer_n) {
				m_circularBuffer_e = 0;
			}
		}

	Error:
		return r;
	}

	size_t NumAvailableBufferBytes() {
		if (m_circularBuffer_e >= m_circularBuffer_c) {
			size_t availBytes = m_circularBuffer_n - m_circularBuffer_e;
			availBytes += m_circularBuffer_c;
			return availBytes;
		}
		else {
			return (m_circularBuffer_c - m_circularBuffer_e);
		}
	}

	
	size_t NumPendingBufferBytes() {
		if (m_circularBuffer_e >= m_circularBuffer_c) {
			return (m_circularBuffer_e - m_circularBuffer_c);
		}
		else {
			size_t diffToEnd = m_circularBuffer_n - m_circularBuffer_c;
			diffToEnd += m_circularBuffer_e;
			return diffToEnd;
		}
	}

	bool IsPendingBufferEmpty() {
		return (m_circularBuffer_e == m_circularBuffer_c) ? true : false;
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

		// increment by buffer size
		m_circularBuffer_c += bytesRead;

		// If overflow, move back to beginning of buffer
		if (m_circularBuffer_c > m_circularBuffer_n) {
			m_circularBuffer_c -= m_circularBuffer_n;
		}

	Error:
		return r;
	}

private:
	CBType m_circularBuffer[MAX_PENDING_BUFFER_LENGTH];

	size_t m_circularBuffer_n = MAX_PENDING_BUFFER_LENGTH;
	size_t m_circularBuffer_e = 0;
	size_t m_circularBuffer_c = 0;
};

#endif // CIRCULAR_BUFFER_H_