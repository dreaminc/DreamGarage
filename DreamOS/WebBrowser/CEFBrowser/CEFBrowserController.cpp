#include "CefBrowserController.h"
//#include "easylogging++.h"

#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"

#include "include/cef_browser.h"
#include "include/cef_app.h"

#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include <sstream>
#include <string>

#include "Cloud/WebRequest.h"
#include "Cloud/WebRequestPostData.h"
#include "Cloud/WebRequestPostDataElement.h"

#include "CEFDOMNode.h"

CEFBrowserController::CEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser) :
	m_pCEFBrowser(pCEFBrowser)
{
	// empty
}

CEFBrowserController::~CEFBrowserController() {
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

RESULT CEFBrowserController::Shutdown() {
	RESULT r = R_PASS;

	if (m_pCEFBrowser != nullptr) {
		m_pCEFBrowser = nullptr;
	}

//Error:
	return r;
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

CefRefPtr<CefPostData> CEFBrowserController::MakeCEFRequestPostData(std::shared_ptr<WebRequestPostData> pWebRequestPostData) {
	RESULT r = R_PASS;

	CefRefPtr<CefPostData> pCEFPostData = CefPostData::Create();
	CN(pCEFPostData);
	CN(pWebRequestPostData);
	
	for (auto &pElement : pWebRequestPostData->GetElements()) {
		CefRefPtr<CefPostDataElement> pCEFPostDataElement = CefPostDataElement::Create();
		CN(pCEFPostDataElement);

		pCEFPostDataElement->SetToBytes(pElement->GetValue().size(), pElement->GetValue().c_str());
		
		CB(pCEFPostData->AddElement(pCEFPostDataElement));
	}

	return pCEFPostData;
Error:

	if (pCEFPostData != nullptr) {
		pCEFPostData = nullptr;
	}

	return nullptr;
}

CefRefPtr<CefRequest> CEFBrowserController::MakeCEFRequest(const WebRequest &webRequest) {
	RESULT r = R_PASS;

	CefRefPtr<CefRequest> pCEFRequest = CefRequest::Create();
	CN(pCEFRequest);
	
	{
		// URL
		pCEFRequest->SetURL((CefString)(static_cast<WebRequest>(webRequest).GetURL().c_str()));

		// Method
		pCEFRequest->SetMethod((CefString)(static_cast<WebRequest>(webRequest).GetRequestMethodString().c_str()));

		// Headers
		// TODO: Might be a more bettarz way to code this
		std::multimap<std::wstring, std::wstring> requestHeasders = static_cast<WebRequest>(webRequest).GetRequestHeaders();

		if (requestHeasders.size() > 0) {
			CefRequest::HeaderMap cefHeaderMap;
			
			for (auto& header: requestHeasders) {
				cefHeaderMap.insert(header);
			}

			pCEFRequest->SetHeaderMap(cefHeaderMap);
		}

		// Post Data
		std::shared_ptr<WebRequestPostData> pWebRequestPostData = static_cast<WebRequest>(webRequest).GetPostData();
		if (pWebRequestPostData != nullptr) {
			CefRefPtr<CefPostData> pCEFPostData = MakeCEFRequestPostData(pWebRequestPostData);
			CN(pCEFPostData);

			pCEFRequest->SetPostData(pCEFPostData);
		}
	}

	return pCEFRequest;

Error:
	if (pCEFRequest != nullptr) {
		pCEFRequest = nullptr;
	}

	return nullptr;
}

RESULT CEFBrowserController::LoadRequest(const WebRequest &webRequest) {
	RESULT r = R_PASS;

	CN(m_pCEFBrowser);

	{
		CefRefPtr<CefRequest> pCEFRequest = MakeCEFRequest(webRequest);
		CN(pCEFRequest);

		pCEFRequest->SetFlags(UR_FLAG_NO_DOWNLOAD_DATA);
		m_pCEFBrowser->GetFocusedFrame()->LoadRequest(pCEFRequest);
	}

Error:
	return r;
}

RESULT CEFBrowserController::SendKeyEventChar(char chKey, bool fKeyDown) {
	RESULT r = R_PASS; 

	CefKeyEvent cefKeyEvent;

	BYTE virtualKeyCode = LOBYTE(VkKeyScanA(chKey));
	UINT scanKeyCode = MapVirtualKey(virtualKeyCode, MAPVK_VK_TO_VSC);

	cefKeyEvent.native_key_code = (scanKeyCode << 16) | 1;
	cefKeyEvent.windows_key_code = virtualKeyCode;

	CN(m_pCEFBrowser);

	if (fKeyDown) {
		///*
		cefKeyEvent.type = KEYEVENT_KEYDOWN;
		m_pCEFBrowser->GetHost()->SendKeyEvent(cefKeyEvent);
		//*/

		///*
		cefKeyEvent.windows_key_code = chKey;
		cefKeyEvent.type = KEYEVENT_CHAR;
		m_pCEFBrowser->GetHost()->SendKeyEvent(cefKeyEvent);
		//*/
	}
	else {
		cefKeyEvent.native_key_code |= 0xC0000000;
		cefKeyEvent.type = KEYEVENT_KEYUP;

		m_pCEFBrowser->GetHost()->SendKeyEvent(cefKeyEvent);
	}

Error:
	return r;
}

RESULT CEFBrowserController::SendKeySequence(const std::string& strKeySequence) {
	RESULT r = R_PASS;

	CefKeyEvent cefKeyEvent;

	for (size_t i = 0; i < strKeySequence.length(); ++i) {
		CR(SendKeyEventChar(strKeySequence[i], true));
		CR(SendKeyEventChar(strKeySequence[i], false));
	}

Error:
	return r;
}

RESULT CEFBrowserController::SendMouseWheel(const WebBrowserMouseEvent& webBrowserMouseEvent, int deltaX, int deltaY) {
	RESULT r = R_PASS;

	CefMouseEvent cefMouseEvent;
	CefBrowserHost::MouseButtonType cefMouseButtonType;
	CN(m_pCEFBrowser);

	cefMouseEvent.x = webBrowserMouseEvent.pt.x;
	cefMouseEvent.y = webBrowserMouseEvent.pt.y;

	cefMouseButtonType = (CefBrowserHost::MouseButtonType)webBrowserMouseEvent.mouseButton;

	m_pCEFBrowser->GetHost()->SendMouseWheelEvent(cefMouseEvent, deltaX, deltaY);

Error:
	return r;
}

RESULT CEFBrowserController::SendMouseClick(const WebBrowserMouseEvent& webBrowserMouseEvent, bool fMouseUp, int clickCount) {
	RESULT r = R_PASS;

	CefMouseEvent cefMouseEvent;
	CefBrowserHost::MouseButtonType cefMouseButtonType;
	CN(m_pCEFBrowser);

	cefMouseEvent.x = webBrowserMouseEvent.pt.x;
	cefMouseEvent.y = webBrowserMouseEvent.pt.y;
	cefMouseEvent.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;

	cefMouseButtonType = (CefBrowserHost::MouseButtonType)webBrowserMouseEvent.mouseButton;

	m_pCEFBrowser->GetHost()->SendMouseClickEvent(cefMouseEvent, cefMouseButtonType, fMouseUp, clickCount);

Error:
	return r;
}


RESULT CEFBrowserController::SendMouseMove(const WebBrowserMouseEvent& webBrowserMouseEvent, bool fMouseLeave) {
	RESULT r = R_PASS;

	CefMouseEvent cefMouseEvent;
	CN(m_pCEFBrowser);

	cefMouseEvent.x = webBrowserMouseEvent.pt.x;
	cefMouseEvent.y = webBrowserMouseEvent.pt.y;

	m_pCEFBrowser->GetHost()->SendMouseMoveEvent(cefMouseEvent, fMouseLeave);

Error:
	return r;
}

// TODO: Mouse wheel
/*--cef()--
virtual void SendMouseWheelEvent(const CefMouseEvent& event,
	int deltaX, int deltaY) = 0;
	*/


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

RESULT CEFBrowserController::OnLoadingStateChanged(bool fLoading, bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFBrowserManager: OnLoadEnd");

	CN(m_pWebBrowserControllerObserver);
	CR(m_pWebBrowserControllerObserver->OnLoadingStateChange(fLoading, fCanGoBack, fCanGoForward));

Error:
	return r;
}

RESULT CEFBrowserController::OnLoadStart(CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFBrowserManager: OnLoadEnd");

	// TODO: Add transition type

	CN(m_pWebBrowserControllerObserver);
	CR(m_pWebBrowserControllerObserver->OnLoadStart());

Error:
	return r;
}

RESULT CEFBrowserController::OnLoadEnd(CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFBrowserManager: OnLoadEnd");

	CN(m_pWebBrowserControllerObserver);
	CR(m_pWebBrowserControllerObserver->OnLoadEnd(httpStatusCode));

Error:
	return r;
}

CefRefPtr<CefBrowser> CEFBrowserController::GetCEFBrowser() {
	return m_pCEFBrowser;
}

RESULT CEFBrowserController::OnFocusedNodeChanged(int cefBrowserID, int cefFrameID, const CEFDOMNode &cefDomNode) {
	RESULT r = R_PASS;

	// Report to browser
	if (m_pWebBrowserControllerObserver != nullptr) {
		CR(m_pWebBrowserControllerObserver->OnNodeFocusChanged(cefDomNode));
	}

Error:
	return r;
}

size_t CEFBrowserController::GetFrameCount() {
	return m_pCEFBrowser->GetFrameCount();
}

/*
// TODO: Put this somewhere better
class CEFDOMVisitor : public CefDOMVisitor {
public:
	CEFDOMVisitor(std::shared_ptr<CEFBrowserController> pParentController) :
		m_pParentCEFBrowserController(pParentController)
	{
		// empty
	}

	virtual void Visit(CefRefPtr<CefDOMDocument> pCefDOMDocument) override {
		RESULT r = R_PASS;

		CN(pCefDOMDocument);
		
		{
			auto pCefDOMNode = pCefDOMDocument->GetFocusedNode();
			CN(pCefDOMNode);
		}

		// catch it

	Error:
		return;
	}

	IMPLEMENT_REFCOUNTING(CEFDOMVisitor);

private:
	std::shared_ptr<CEFBrowserController> m_pParentCEFBrowserController = nullptr;
};

// TODO: Not sure if we need this
RESULT CEFBrowserController::GetFocusedNode() {
	RESULT r = R_PASS;

	// Create the message object.
	CefRefPtr<CefProcessMessage> pCEFProcessMessage = CefProcessMessage::Create("CEFBrowserController::GetFocusedNode");

	// Retrieve the argument list object.
	CefRefPtr<CefListValue> cefMessageArgs = pCEFProcessMessage->GetArgumentList();

	// Populate the argument values.
	cefMessageArgs->SetString(0, "TestString");
	cefMessageArgs->SetInt(0, 55);

	// Send the process message to the render process.
	// Use PID_BROWSER instead when sending a message to the browser process.
	CN(m_pCEFBrowser);
	CBM((m_pCEFBrowser->SendProcessMessage(PID_RENDERER, pCEFProcessMessage)), "Failed to send browser process a message");

Error:
	return r;
}
*/