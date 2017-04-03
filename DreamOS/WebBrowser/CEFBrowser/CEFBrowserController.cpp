#include "CefBrowserController.h"
//#include "easylogging++.h"

#include "include/cef_browser.h"

#include <sstream>
#include <string>

CEFBrowserController::CEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser) :
	m_pCEFBrowser(pCEFBrowser)
{
	// empty
}

RESULT CEFBrowserController::RegisterCEFAppObserver(CEFBrowserController::observer* pCEFBrowserControllerObserver) {
	RESULT r = R_PASS;

	CBM((m_pWebBrowserControllerObserver == nullptr), "CEFBrowserControllerObserver already registered");
	CN(pCEFBrowserControllerObserver);

	m_pWebBrowserControllerObserver = pCEFBrowserControllerObserver;

Error:
	return r;
}

RESULT CEFBrowserController::PollFrame() {
	RESULT r = R_PASS;

	std::unique_lock<std::mutex> lockBufferMutex(m_BufferMutex);
	
	if (m_pWebBrowserControllerObserver != nullptr) {
		WebBrowserRect rect = { 0, 0, m_bufferWidth, m_bufferHeight };
		CR(m_pWebBrowserControllerObserver->OnPaint(rect, &m_vectorBuffer[0], m_bufferWidth, m_bufferHeight));
	}

Error:
	return r;
}

RESULT CEFBrowserController::PollNewDirtyFrames(int &rNumFramesProcessed) {
	RESULT r = R_PASS;
	
	std::unique_lock<std::mutex> lockBufferMutex(m_BufferMutex);

	rNumFramesProcessed = 0;

	if (m_pWebBrowserControllerObserver != nullptr) {
		for (auto& dirtyFrame : m_NewDirtyFrames) {
			WebBrowserRect rect = { dirtyFrame.x, dirtyFrame.y, dirtyFrame.width, dirtyFrame.height };
			
			CR(m_pWebBrowserControllerObserver->OnPaint(rect, &m_vectorBuffer[0], m_bufferWidth, m_bufferHeight));
		
			rNumFramesProcessed++;
		}
	}

	m_NewDirtyFrames.clear();

Error:
	return r;
}

RESULT CEFBrowserController::Resize(unsigned int width, unsigned int height) {
	RESULT r = R_PASS;

	CN(m_pCEFBrowser);

	m_browserWidth = width;
	m_browserHeight = height;

	m_pCEFBrowser->GetHost()->WasResized();

Error:
	return r;
}

RESULT CEFBrowserController::LoadURL(const std::string& url) {
	RESULT r = R_PASS;

	CN(m_pCEFBrowser);

	m_pCEFBrowser->GetFocusedFrame()->LoadURL(url);

Error:
	return r;
}

RESULT CEFBrowserController::SendKeySequence(const std::string& keys) {
	RESULT r = R_PASS;

	size_t word_length = keys.length();
	CefKeyEvent eventToTest;

	CN(m_pCEFBrowser);

	for (size_t i = 0; i < word_length; ++i) {
		BYTE VkCode = LOBYTE(VkKeyScanA(keys[i]));
		UINT scanCode = MapVirtualKey(VkCode, MAPVK_VK_TO_VSC);

		eventToTest.native_key_code = (scanCode << 16) | 1;
		eventToTest.windows_key_code = VkCode;
		eventToTest.type = KEYEVENT_RAWKEYDOWN;
		
		m_pCEFBrowser->GetHost()->SendKeyEvent(eventToTest);

		eventToTest.windows_key_code = keys[i];
		eventToTest.type = KEYEVENT_CHAR;
		
		m_pCEFBrowser->GetHost()->SendKeyEvent(eventToTest);

		eventToTest.windows_key_code = VkCode;
		eventToTest.native_key_code |= 0xC0000000;
		eventToTest.type = KEYEVENT_KEYUP;

		m_pCEFBrowser->GetHost()->SendKeyEvent(eventToTest);
	}

Error:
	return r;
}


RESULT CEFBrowserController::OnGetViewRect(CefRect &cefRect){
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFBrowserController: GetViewRect");

	cefRect = CefRect(0, 0, m_browserWidth, m_browserHeight);

//Error:
	return r;
}

RESULT CEFBrowserController::OnPaint(CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFBrowserManager: OnPaint");

	std::unique_lock<std::mutex> lockBufferMutex(m_BufferMutex);

	size_t pBuffer_n = width * height * 4;

	m_vectorBuffer.assign(static_cast<const unsigned char*>(pBuffer), static_cast<const unsigned char*>(pBuffer) + pBuffer_n);

	bool fSizeChanged = (width != m_bufferWidth) || (height != m_bufferHeight);

	if (fSizeChanged) {
		m_bufferWidth = width;
		m_bufferHeight = height;
		DEBUG_LINEOUT("Size changed to w:%d h:%d", m_bufferWidth, m_bufferHeight);
	}

	m_NewDirtyFrames.insert(m_NewDirtyFrames.end(), dirtyRects.begin(), dirtyRects.end());

//Error:
	return r;
}

CefRefPtr<CefBrowser> CEFBrowserController::GetCEFBrowser() {
	return m_pCEFBrowser;
}