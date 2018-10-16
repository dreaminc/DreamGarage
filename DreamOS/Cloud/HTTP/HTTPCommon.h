#ifndef HTTP_COMMON_H_
#define HTTP_COMMON_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/HTTP/HTTPCommon.h
// Common HTTP Decelerations for the HTTP controller and associated objects

#include <functional>
#include <string>
#include <vector>
#include <memory>

typedef std::function<RESULT(std::string&&)> HTTPResponseCallback;
typedef std::function<RESULT()> HTTPTimeoutCallback;
typedef std::function<RESULT(std::shared_ptr<std::vector<uint8_t>>, void*)> HTTPResponseFileCallback;

#endif // ! HTTP_COMMON_H_