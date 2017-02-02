#include "CefBrowser.h"
#include "easylogging++.h"


CefBrowserServiceImp::CefBrowserServiceImp() {

}

CefBrowserServiceImp::~CefBrowserServiceImp() {
	UnInitialize();
}

RESULT CefBrowserServiceImp::Initialize() {
	RESULT r = R_PASS;

	LOG(INFO) << "Initializing Cef thread..";

	m_ServiceThread = std::thread(&CefBrowserServiceImp::ServiceThread, this);

	// wait for Cef to initialize

	std::unique_lock<std::mutex> lk(m_Mutex);
	m_BrowserInit.wait(lk, [&] {return (m_InitState != InitState::Initializing); });

	LOG(INFO) << "Initializing Cef thread..";

	switch (m_InitState)
	{
	case CefBrowserServiceImp::InitState::Initializing:
		LOG(ERROR) << "Cef initialize failed (Initializing state)";
		r = R_FAIL;
		break;
	case CefBrowserServiceImp::InitState::Initialized:
		LOG(INFO) << "Cef initialize complete";
		r = R_PASS;
		break;
	case CefBrowserServiceImp::InitState::Failed:
		LOG(ERROR) << "Cef initialize failed (Failed state)";
		r = R_FAIL;
		break;
	default:
		break;
	}

	return r;
}

void CefBrowserServiceImp::UnInitialize() {
	LOG(INFO) << "Cef force shutdown";

	// delete all opened browser will also shut down cef thread
	m_OpenBrowsers.clear();

	CefQuitMessageLoop();

	m_ServiceThread.join();
}

void CefBrowserServiceImp::ServiceThread() {
	CefSettings m_CEFSettings;

	const CefMainArgs CEFMainArgs(GetModuleHandle(NULL));

	void* CEFSandboxInfo = nullptr;

	int exitCode = CefExecuteProcess(CEFMainArgs, nullptr, nullptr);

	LOG(INFO) << "CefExecuteProcess returned " << exitCode;

	// Initialize CEF.
	CefString(&m_CEFSettings.browser_subprocess_path) = k_CefProcessName;
	CefString(&m_CEFSettings.locale) = "en";

	if (!CefInitialize(CEFMainArgs, m_CEFSettings, nullptr, nullptr)) {
		LOG(ERROR) << "CefInitialized faild.";

		m_InitState = InitState::Failed;
		m_BrowserInit.notify_one();
		return;
	}

	LOG(INFO) << "CefInitialize completed successfully";

	m_InitState = InitState::Initialized;
	m_BrowserInit.notify_one();
	
	// Cef will need at least one browser to run in order to process its message loop
	// wait for at least one browser to run
	LOG(INFO) << "Waiting for at least one browser to continue";

	{
		std::unique_lock<std::mutex> lk(m_Mutex);
		m_BrowserReady.wait(lk, [&] {return m_IsReady; });
	}

	LOG(INFO) << "Run message loop";

	CefRunMessageLoop();

	LOG(INFO) << "Shutting down...";

	// shut down

	CefShutdown();

	LOG(INFO) << "Exited";
}

std::shared_ptr<WebBrowserController> CefBrowserServiceImp::CreateNewWebBrowser(const std::string& url, unsigned int width, unsigned int height) {
	auto browser = new CefBufferedBrowserImp(width, height);

	CefWindowInfo window_info;
	CefBrowserSettings browserSettings;

	window_info.SetAsWindowless(0, false);

	if (!CefBrowserHost::CreateBrowser(window_info, browser, url, browserSettings, nullptr)) {
		LOG(ERROR) << "CreateBrowser failed, reason unknown";
		return nullptr;
	}

	LOG(INFO) << "CreateBrowser succeeded, signal to browser service";

	{
		std::lock_guard<std::mutex> lk(m_Mutex);
		m_IsReady = true;
	
	}
	m_BrowserReady.notify_one();

	m_OpenBrowsers.push_back(std::shared_ptr<WebBrowserController>(browser));

	LOG(INFO) << "CreateNewWebBrowser succeeded";

	return m_OpenBrowsers.back();
}


//CefBrowserImp

CefBrowserImp::~CefBrowserImp() {

}

CefRefPtr<CefRenderHandler> CefBrowserImp::GetRenderHandler() {
	return this;
}

bool CefBrowserImp::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
	rect = CefRect();
	return true;
}

void CefBrowserImp::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) {

}

void CefBrowserImp::PollFrame(std::function<bool(unsigned char *output, unsigned int width, unsigned int height)> pred) {

}

int CefBrowserImp::PollNewDirtyFrames(std::function<bool(unsigned char *output, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)> pred) {
	return 0;
}

// CefBufferedBrowserImp

CefBufferedBrowserImp::CefBufferedBrowserImp(unsigned int width, unsigned int height) :
	m_width(width),
	m_height(height) {

}

CefBufferedBrowserImp::~CefBufferedBrowserImp() {

}

CefRefPtr<CefRenderHandler> CefBufferedBrowserImp::GetRenderHandler() {
	return this;
}

bool CefBufferedBrowserImp::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
	rect = CefRect(0, 0, m_width, m_height);
	return true;
}

void CefBufferedBrowserImp::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) {
	std::lock_guard<std::mutex> lock(m_BufferMutex);

	unsigned int bufferSize = width * height * 4;

	const unsigned char* pBuffer = static_cast<const unsigned char*>(buffer);

	m_FrontBuffer.assign(pBuffer, pBuffer + bufferSize);

	bool isSizeChanged = (width != m_width) || (height != m_height);

	if (isSizeChanged) {
		m_width = width;
		m_height = height;
		LOG(INFO) << "Size changed to " << m_width << "," << m_height;
	}

	m_NewDirtyFrames.insert(m_NewDirtyFrames.end(), dirtyRects.begin(), dirtyRects.end());
}

void CefBufferedBrowserImp::PollFrame(std::function<bool(unsigned char *output, unsigned int width, unsigned int height)> pred) {
	std::lock_guard<std::mutex> lock(m_BufferMutex);
	pred(&m_FrontBuffer[0], m_width, m_height);
}

int CefBufferedBrowserImp::PollNewDirtyFrames(std::function<bool(unsigned char *output, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom)> pred) {
	std::lock_guard<std::mutex> lock(m_BufferMutex);

	for (auto& dirtyFrame : m_NewDirtyFrames) {
		if (false == pred(&m_FrontBuffer[0], m_width, m_height, dirtyFrame.x, dirtyFrame.y, dirtyFrame.width, dirtyFrame.height))
			break;
	}

	int numberOfFrames = static_cast<int>(m_NewDirtyFrames.size());

	m_NewDirtyFrames.clear();

	return numberOfFrames;
}

