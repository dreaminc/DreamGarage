#ifndef HTTP_COMMON_H_
#define HTTP_COMMON_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/HTTP/HTTPCommon.h
// Common HTTP Decelerations for the HTTP controller and associated objects

#include <functional>
#include <string>

typedef std::function<void(std::string&&)> HTTPResponseCallback;
typedef std::function<RESULT(uint8_t*, size_t)> HTTPResponseFileCallback;

#endif // ! HTTP_COMMON_H_