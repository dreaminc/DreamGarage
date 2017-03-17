#include "HTTPRequestFileHandler.h"
#include "Sandbox/PathManager.h"
#include "Core/Utilities.h"

HTTPRequestFileHandler::HTTPRequestFileHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback) :
	HTTPRequestHandler(pHTTPRequest, pHTTPResponse, fnResponseCallback)
{ 
	m_pFile_bytes = 0;
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

RESULT HTTPRequestFileHandler::HandleHTTPResponse(char *pBuffer, size_t elementSize, size_t numElements) {
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