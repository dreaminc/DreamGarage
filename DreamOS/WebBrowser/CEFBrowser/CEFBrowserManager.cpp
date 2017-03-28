#include "CEFBrowserManager.h"

#include "CEFBrowserController.h"
#include "CEFBrowserService.h"

#include "DreamConsole/DreamConsole.h"

RESULT CEFBrowserManager::Initialize(composite* composite) {
	RESULT r = R_PASS;

	m_pComposite = composite;
	m_pCEFBrowserService = std::make_unique<CEFBrowserService>();

	// Initialize
	CR(m_pCEFBrowserService->Initialize());

Error:
	return r;
}

void CEFBrowserManager::Update() {
	for (auto& b : m_Browsers) {
		// TODO: optimize with actual dirty rects copy
		if (b.second.pCEFBrowserController->PollNewDirtyFrames([&](unsigned char *output, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom) -> bool {
			b.second.pTexture->Update(output, width, height, texture::PixelFormat::BGRA);
			// poll whole frame and stop iterations
			return false;
		})) {
		}
	}
}

// TODO: all of this into DreamBrowser
std::string CEFBrowserManager::CreateNewBrowser(unsigned int width, unsigned int height, const std::string& url) {
	static int id = 0;
	id++;

	// TODO: remove this
	BrowserObject browserObject {
		(CEFBrowserController*)(m_pCEFBrowserService->CreateNewWebBrowser(url, width, height)),
		nullptr,
		nullptr
	};

	if (browserObject.pCEFBrowserController == nullptr) {
		return "";
	}

	std::vector<unsigned char>	buffer(width * height * 4, 0);

	browserObject.pTexture = m_pComposite->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_COLOR, width, height, texture::PixelFormat::RGBA, 4, &buffer[0], width * height * 4);

	float quadWidth = 4.0f * width / 512;
	float quadHeight = 4.0f * height / 512;

	std::shared_ptr<quad> quad = m_pComposite->AddQuad(quadWidth, quadHeight);

	// Vertical flip
	quad->TransformUV(
		{ { 0, 0 } }, 
		{ { 1, 0, 
			0, -1 } }
	);

	quad->ResetRotation();
	quad->RotateXBy(0.3f);

	// TODO: this is dumb
	static float xOffset = -quadWidth / 2;

	quad->MoveTo(xOffset + quadWidth / 2, -0.3f + 0.8f, 0);

	xOffset += quadWidth;

	quad->GetMaterial()->Set(color(0.5f, 0.5f, 0.5f, 1.0f), color(0.5f, 0.5f, 0.5f, 1.0f), color(0.0f, 0.0f, 0.0f, 1.0f));

	quad->SetMaterialTexture(DimObj::MaterialTexture::Ambient, browserObject.pTexture.get());
	quad->SetMaterialTexture(DimObj::MaterialTexture::Diffuse, browserObject.pTexture.get());

	m_Browsers[std::to_string(id)] = browserObject;

	HUD_OUT("created browser id = %d (%dx%d)", id, width, height);

	return std::to_string(id);
}

CEFBrowserController* CEFBrowserManager::GetBrowser(const std::string& strID) {
	if (m_Browsers.find(strID) != m_Browsers.end()) {
		return m_Browsers[strID].pCEFBrowserController;
	}

	return nullptr;
}

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