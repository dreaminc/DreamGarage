#include "HTTPRequestFileHandler.h"
#include "Sandbox/PathManager.h"
#include "Core/Utilities.h"

HTTPRequestFileHandler::HTTPRequestFileHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseCallback fnResponseCallback) :
	HTTPRequestHandler(pHTTPRequest, pHTTPResponse, fnResponseCallback)
{ 
	// empty
}

RESULT HTTPRequestFileHandler::SetDestinationFilePath(std::wstring wstrDestinationFilepath) {
	m_wstrDestinationFilePath = wstrDestinationFilepath;
	return R_PASS;
}

RESULT HTTPRequestFileHandler::HandleHTTPResponse(char *pBuffer, size_t elementSize, size_t numElements) {
	RESULT r = R_PASS;

	PathManager* pPathManager = PathManager::instance();
	FILE *pFILE = nullptr;
	size_t pBuffer_n = elementSize * numElements;

	CN(pBuffer);
	CN(pPathManager);
	CB(pBuffer_n > 0);

	// Save to file
	
	if (pPathManager->DoesPathExist(m_wstrDestinationFilePath, false) == R_FILE_FOUND) {
		// Delete or overwrite the file?
	}

	// Save to file

	// TODO: Note: This will kill the file if it's there
	pFILE = fopen(util::WideStringToString(m_wstrDestinationFilePath).c_str(), "wb");
	CN(pFILE);
	{
		size_t bytesWritten = fwrite(pBuffer, elementSize, numElements, pFILE);
		CBM((bytesWritten == pBuffer_n), "Failed to write file %zd bytes written differ from buffer size %zd", bytesWritten, pBuffer_n);
	}


Error:
	if (pFILE != nullptr) {
		fclose(pFILE);
		pFILE = nullptr;
	}

	return r;
}