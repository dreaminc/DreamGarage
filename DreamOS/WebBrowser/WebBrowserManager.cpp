#include "WebBrowserManager.h"

#include "WebBrowserController.h"

WebBrowserManager::WebBrowserManager() {
	// empty
}

WebBrowserManager::~WebBrowserManager() {
	RESULT r = R_PASS;

	//CRM(Shutdown(), "WebBrowserManager failed to shutdown properly");
	CR(r);

Error:
	return;
}

// TODO: all of this into DreamBrowser
std::shared_ptr<WebBrowserController> WebBrowserManager::CreateNewBrowser(int width, int height, const std::string& strURL) {
	RESULT r = R_PASS;
	std::shared_ptr<WebBrowserController> pWebBrowserController = nullptr;

	// This will go to the implementation specific make function
	pWebBrowserController = MakeNewBrowser(width, height, strURL);
	CN(pWebBrowserController);

	m_webBrowserControllers.push_back(pWebBrowserController);

// Success:
	return pWebBrowserController;

Error:
	if (pWebBrowserController != nullptr) {
		pWebBrowserController = nullptr;
	}

	return nullptr;
}

	/*
	static int id = 0;
	id++;

	std::string strID = std::to_string(id);

	// TODO: remove this
	BrowserObject browserObject {
		(CEFBrowserController*)(m_pCEFBrowserService->CreateNewWebBrowser(strURL, width, height)),
		nullptr,
		nullptr
	};

	// TODO: Replace with CN
	if (browserObject.pCEFBrowserController == nullptr) {
		return "";
	}

	//std::vector<unsigned char> buffer(width * height * 4, 0);
	unsigned char *pBuffer = nullptr;
	size_t pBuffer_n = sizeof(unsigned char) * (width * height * 4);
	pBuffer = (unsigned char*)malloc(pBuffer_n);
	memset(pBuffer, 0xFF, pBuffer_n);

	browserObject.pTexture = m_pComposite->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_COLOR, width, height, texture::PixelFormat::RGBA, 4, pBuffer, (int)(pBuffer_n));

	float quadWidth = 4.0f * width / 512;
	float quadHeight = 4.0f * height / 512;

	browserObject.pQuad = m_pComposite->AddQuad(quadWidth, quadHeight);
	browserObject.pQuad->SetVisible(true);

	// Vertical flip
	browserObject.pQuad->TransformUV(
		{ { 0, 0 } }, 
		{ { 1, 0, 
			0, -1 } }
	);

	browserObject.pQuad->ResetRotation();
	browserObject.pQuad->RotateXBy(0.3f);

	// TODO: this is dumb
	static float xOffset = -quadWidth / 2;

	browserObject.pQuad->MoveTo(xOffset + quadWidth / 2, -0.3f + 0.8f, 0);

	xOffset += quadWidth;

	browserObject.pQuad->GetMaterial()->Set(color(0.5f, 0.5f, 0.5f, 1.0f), color(0.5f, 0.5f, 0.5f, 1.0f), color(0.0f, 0.0f, 0.0f, 1.0f));

	browserObject.pQuad->SetMaterialTexture(DimObj::MaterialTexture::Ambient, browserObject.pTexture.get());
	browserObject.pQuad->SetMaterialTexture(DimObj::MaterialTexture::Diffuse, browserObject.pTexture.get());

	m_Browsers[strID] = browserObject;

	DEBUG_LINEOUT("Created browser id = %d (%dx%d)", id, width, height);

	return strID;
*/

RESULT WebBrowserManager::ClearAllBrowserControllers() {
	m_webBrowserControllers.clear();
	return R_PASS;
}

std::shared_ptr<WebBrowserController> WebBrowserManager::GetBrowser(const std::string& strID) {
	for (auto &pWebBrowserController : m_webBrowserControllers) {
		if (pWebBrowserController->GetID() == strID) {
			return pWebBrowserController;
		}
	}

	return nullptr;
}

RESULT WebBrowserManager::RemoveBrowser(std::shared_ptr<WebBrowserController> pWebBrowserController) {
	m_webBrowserControllers.remove(pWebBrowserController);
	return R_PASS;
}

/*
void CEFBrowserManager::SetKeyFocus(const std::string& id) {
	m_browserInKeyFocus = GetBrowser(id);
}

void CEFBrowserManager::OnKey(unsigned int scanCode, char16_t chr) {
	if (m_browserInKeyFocus) {
		if (scanCode == VK_ESCAPE) {
			m_browserInKeyFocus = nullptr;
			HUD_OUT("browser control is released");
		}
		else {
			// Process displayable characters. 
			std::string nonUnicodeChar = utf16_to_utf8(std::u16string(1, chr));

			m_browserInKeyFocus->SendKeySequence(nonUnicodeChar);
		}
	}
}
*/