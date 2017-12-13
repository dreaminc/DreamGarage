#ifndef CEF_RESOURCE_HANDLER_H_
#define CEF_RESOURCE_HANDLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CefHandler.h

#ifdef LOG
#undef LOG
#endif

#ifdef PLOG
#undef PLOG
#endif

#include "Primitives/singleton.h"

#include "WebBrowser/WebBrowserController.h"

#include "CEFBrowserController.h"

#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"

#include <thread>
#include <mutex>
#include <condition_variable>

#include <list>
#include <future>

class CEFResourceHandler
{
public:
	CEFResourceHandler();
	~CEFResourceHandler();
};

