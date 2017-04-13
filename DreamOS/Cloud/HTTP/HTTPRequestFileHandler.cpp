#include "HTTPRequestFileHandler.h"
#include "Sandbox/PathManager.h"
#include "Core/Utilities.h"

HTTPRequestFileHandler::HTTPRequestFileHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseFileCallback fnResponseFileCallback) :
	HTTPRequestHandler(pHTTPRequest, pHTTPResponse, nullptr),
	m_fnResponseFileCallback(fnResponseFileCallback)
{ 
	m_pFile_bytes = 0;

	m_pBufferVector = std::make_shared<std::vector<uint8_t>>();
}

HTTPRequestFileHandler::~HTTPRequestFileHandler() {
	//
}

RESULT HTTPRequestFileHandler::SetDestinationFilePath(std::wstring wstrDestinationFilepath) {
	m_wstrDestinationFilePath = wstrDestinationFilepath;
	return R_PASS;
}

// TODO: Note: This will overwrite the file if it's there - add capability?
RESULT HTTPRequestFileHandler::OpenFilePath() {
	RESULT r = R_PASS;

	PathManager* pPathManager = PathManager::instance();
	CN(pPathManager);

	CB((m_pFILE == nullptr));

	// Save to file
	if (pPathManager->DoesPathExist(m_wstrDestinationFilePath, false) == R_FILE_FOUND) {
		// Delete or overwrite the file?
	}

	// Save to file
	if (m_pFile_bytes == 0) {
		m_pFILE = fopen(util::WideStringToString(m_wstrDestinationFilePath).c_str(), "wb+");
	}
	else {
		m_pFILE = fopen(util::WideStringToString(m_wstrDestinationFilePath).c_str(), "ab");
	}
	CN(m_pFILE);

Error:
	return r;
}

RESULT HTTPRequestFileHandler::SaveBufferToFilePath(char *pBuffer, size_t elementSize, size_t numElements) {
	RESULT r = R_PASS;

	size_t pBuffer_n = elementSize * numElements;
	CN(pBuffer);
	CB(pBuffer_n > 0);

	if (m_pFILE == nullptr) {
		CR(OpenFilePath());
	}

	{
		size_t bytesWritten = fwrite(pBuffer, elementSize, numElements, m_pFILE);
		CBM((bytesWritten == pBuffer_n), "Failed to write file %zd bytes written differ from buffer size %zd", bytesWritten, pBuffer_n);
		m_pFile_bytes += bytesWritten;

		DEBUG_LINEOUT("read %zd bytes, total: %zd", bytesWritten, m_pFile_bytes);
	}

Error:
	if (m_pFILE != nullptr) {
		fclose(m_pFILE);
		m_pFILE = nullptr;
	}

	return r;
}

RESULT HTTPRequestFileHandler::OnHTTPRequestComplete() {
	RESULT r = R_PASS;

	if (m_fnResponseFileCallback != nullptr) {
		uint8_t *pBuffer = GetBuffer();
		size_t pBuffer_n = GetBufferSize();

		CR(m_fnResponseFileCallback(m_pBufferVector));
	}

Error:
	return r;
}

RESULT HTTPRequestFileHandler::HandleHTTPResponse(char *pBuffer, size_t elementSize, size_t numElements) {
	RESULT r = R_PASS;

	size_t pBuffer_n = elementSize * numElements;

	CN(pBuffer);
	CB(pBuffer_n > 0);

	DEBUG_LINEOUT("buffer size %zd", pBuffer_n);

	if (m_wstrDestinationFilePath.length() > 0) {
		// Save to File
		CR(SaveBufferToFilePath(pBuffer, elementSize, numElements));
	}
	else {
		// Save (append) to buffer
		CR(AppendToBuffer(pBuffer, elementSize, numElements));
	}

Error:
	return r;
}

RESULT HTTPRequestFileHandler::ResetBuffer() {
	m_pBufferVector->clear();
	return R_PASS;
}

RESULT HTTPRequestFileHandler::AppendToBuffer(char *pBuffer, size_t elementSize, size_t numElements) {
	RESULT r = R_PASS;

	size_t pBuffer_n = elementSize * numElements;

	CN(pBuffer);
	CB(pBuffer_n > 0);

	{
		std::vector<uint8_t> newBufferVector(pBuffer, pBuffer + (pBuffer_n));
		m_pBufferVector->insert(m_pBufferVector->end(), newBufferVector.begin(), newBufferVector.end());
	}

Error:
	return r;
}


uint8_t* HTTPRequestFileHandler::GetBuffer() {
	uint8_t* pBuffer = &(m_pBufferVector->operator[](0));
	return pBuffer;
}

size_t HTTPRequestFileHandler::GetBufferSize() {
	return m_pBufferVector->size();
}