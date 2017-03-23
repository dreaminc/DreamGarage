#ifndef HTTP_REQUEST_FILE_HANDLER_H_
#define HTTP_REQUEST_FILE_HANDLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Objects/User.h
// Base User object

#include <functional>
#include <vector>

#include "HTTPRequestHandler.h"

class HTTPRequest;
class HTTPResponse;

class HTTPRequestFileHandler : public HTTPRequestHandler {
public:
	HTTPRequestFileHandler(HTTPRequest* pHTTPRequest, HTTPResponse* pHTTPResponse, HTTPResponseFileCallback fnResponseFileCallback);	
	~HTTPRequestFileHandler();

	RESULT SetDestinationFilePath(std::wstring wstrDestinationFilepath);
	virtual RESULT HandleHTTPResponse(char *pBuffer, size_t elementSize, size_t numElements) override;
	virtual RESULT OnHTTPRequestComplete() override;

private:
	RESULT OpenFilePath();
	RESULT SaveBufferToFilePath(char *pBuffer, size_t elementSize, size_t numElements);

	RESULT AppendToBuffer(char *pBuffer, size_t elementSize, size_t numElements);
	RESULT ResetBuffer();
	uint8_t *GetBuffer();
	size_t GetBufferSize();

private:
	std::wstring m_wstrDestinationFilePath;

	// Save to file
	FILE *m_pFILE = nullptr;
	size_t m_pFile_bytes = 0;

	// Save to buffer
	std::vector<uint8_t> m_bufferVector;

	HTTPResponseFileCallback m_fnResponseFileCallback = nullptr;
};


#endif	// ! HTTP_REQUEST_FILE_HANDLER_H_