#include "CefBrowserController.h"
#include "easylogging++.h"

#include "include/cef_browser.h"

#include <sstream>
#include <string>

CEFBrowserController::CEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser) :
	m_pCEFBrowser(pCEFBrowser)
{
	// empty
}

RESULT CEFBrowserController::PollFrame() {
	RESULT r = R_PASS;

	std::unique_lock<std::mutex> lock(m_BufferMutex);
	//fnPred(&m_buffer[0], m_bufferWidth, m_bufferHeight);

//Error:
	return r;
}

int CEFBrowserController::PollNewDirtyFrames() {
	RESULT r = R_PASS;
	
	std::unique_lock<std::mutex> lock(m_BufferMutex);

	int numberOfFrames = static_cast<int>(m_NewDirtyFrames.size());

	/*
	for (auto& dirtyFrame : m_NewDirtyFrames) {
		if (false == fnPred(&m_buffer[0], m_bufferWidth, m_bufferHeight, dirtyFrame.x, dirtyFrame.y, dirtyFrame.width, dirtyFrame.height)) {
			break;
		}
	}
	*/

	m_NewDirtyFrames.clear();

//Error:
	return numberOfFrames;
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

// CefRenderHandler

bool CEFBrowserController::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &cefRect) {
	cefRect = CefRect(0, 0, m_browserWidth, m_browserHeight);
	return true;
}

void CEFBrowserController::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) {
	std::unique_lock<std::mutex> lock(m_BufferMutex);

	unsigned int bufferSize = width * height * 4;

	const unsigned char* pBuffer = static_cast<const unsigned char*>(buffer);

	m_buffer.assign(pBuffer, pBuffer + bufferSize);

	bool isSizeChanged = (width != m_bufferWidth) || (height != m_bufferHeight);

	if (isSizeChanged) {
		m_bufferWidth = width;
		m_bufferHeight = height;
		LOG(INFO) << "Size changed to " << m_bufferWidth << "," << m_bufferHeight;
	}

	m_NewDirtyFrames.insert(m_NewDirtyFrames.end(), dirtyRects.begin(), dirtyRects.end());
}

