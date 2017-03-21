#include "CefBrowserController.h"
#include "easylogging++.h"

#include "include/cef_browser.h"

#include <sstream>
#include <string>

void CefBrowserController::PollFrame(std::function<bool(unsigned char *output, unsigned int width, unsigned int height)> pred) {
	std::unique_lock<std::mutex> lock(m_BufferMutex);
	pred(&m_buffer[0], m_bufferWidth, m_bufferHeight);
}

int CefBrowserController::PollNewDirtyFrames(std::function<bool(unsigned char *output, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)> pred) {
	std::unique_lock<std::mutex> lock(m_BufferMutex);

	for (auto& dirtyFrame : m_NewDirtyFrames) {
		if (false == pred(&m_buffer[0], m_bufferWidth, m_bufferHeight, dirtyFrame.x, dirtyFrame.y, dirtyFrame.width, dirtyFrame.height))
			break;
	}

	int numberOfFrames = static_cast<int>(m_NewDirtyFrames.size());

	m_NewDirtyFrames.clear();

	return numberOfFrames;
}

bool CefBrowserController::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
	rect = CefRect(0, 0, m_browserWidth, m_browserHeight);
	return true;
}

void CefBrowserController::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) {
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

void CefBrowserController::Resize(unsigned int width, unsigned int height) {
	m_browserWidth = width;
	m_browserHeight = height;

	m_browser->GetHost()->WasResized();
}

void CefBrowserController::LoadURL(const std::string& url) {
	m_browser->GetFocusedFrame()->LoadURL(url);
}

void CefBrowserController::SendKeySequence(const std::string& keys) {
	size_t word_length = keys.length();
	CefKeyEvent eventToTest;

	for (size_t i = 0; i < word_length; ++i) {
		BYTE VkCode = LOBYTE(VkKeyScanA(keys[i]));
		UINT scanCode = MapVirtualKey(VkCode, MAPVK_VK_TO_VSC);

		eventToTest.native_key_code = (scanCode << 16) | 1;
		eventToTest.windows_key_code = VkCode;
		eventToTest.type = KEYEVENT_RAWKEYDOWN;
		m_browser->GetHost()->SendKeyEvent(eventToTest);

		eventToTest.windows_key_code = keys[i];
		eventToTest.type = KEYEVENT_CHAR;
		m_browser->GetHost()->SendKeyEvent(eventToTest);

		eventToTest.windows_key_code = VkCode;
		eventToTest.native_key_code |= 0xC0000000;
		eventToTest.type = KEYEVENT_KEYUP;
		m_browser->GetHost()->SendKeyEvent(eventToTest);
	}
}

