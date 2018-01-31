#include "DreamShareView.h" 
#include "DreamOS.h"

#include "Primitives/quad.h"
#include "Primitives/texture.h"

RESULT DreamShareViewHandle::SendCastingEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ShowCastingTexture());
Error:
	return r;
}

RESULT DreamShareViewHandle::SendLoadingEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ShowLoadingTexture());
Error:
	return r;
}

RESULT DreamShareViewHandle::SendShowEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(Show());
Error:
	return r;
}

RESULT DreamShareViewHandle::SendHideEvent() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(Hide());
Error:
	return r;
}

RESULT DreamShareViewHandle::SendCastTexture(std::shared_ptr<texture> pNewCastTexture) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(SetCastingTexture(pNewCastTexture));
Error:
	return r;
}

DreamShareView::DreamShareView(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamShareView>(pDreamOS, pContext)
{
	// Empty
}

DreamShareView::~DreamShareView() {
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

RESULT DreamShareView::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	SetAppName("DreamShareView");

	GetComposite()->SetPosition(point(0.0f, 2.0f, -2.0f));

	int channels = 4;
	int pxSize = m_castpxWidth * m_castpxHeight * channels;
	std::vector<unsigned char> vectorByteBuffer(pxSize, 0xFF);

	// set up the casting quad
	m_aspectRatio = ((float)m_castpxWidth / (float)m_castpxHeight);
	float castWidth = std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	float castHeight = std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));
	vector vNormal = vector(0.0f, 0.0f, 1.0f).Normal();

	m_pCastQuad = GetComposite()->AddQuad(castWidth, castHeight, 1, 1, nullptr, vNormal);
	CN(m_pCastQuad);

	m_pCastQuad->SetMaterialAmbient(0.90f);
	m_pCastQuad->FlipUVVertical();
	CR(m_pCastQuad->SetVisible(false));

	m_pCastTexture = GetComposite()->MakeTexture(
		texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, 
		m_castpxWidth, 
		m_castpxHeight, 
		PIXEL_FORMAT::RGBA, 
		channels, 
		&vectorByteBuffer[0], 
		pxSize);	
	CN(m_pCastTexture);

	m_pLoadingTexture = std::shared_ptr<texture>(GetDOS()->MakeTexture(k_wszLoadingScreen, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	CN(m_pLoadingTexture);

	m_pCastQuad->SetDiffuseTexture(m_pLoadingTexture.get());

Error:
	return r;
}

RESULT DreamShareView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamShareView::Update(void *pContext) {
	return R_PASS;
}

RESULT DreamShareView::Shutdown(void *pContext) {
	return R_PASS;
}

DreamShareView* DreamShareView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamShareView *pDreamApp = new DreamShareView(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamShareView::ShowLoadingTexture() {
	m_pCastQuad->SetDiffuseTexture(m_pLoadingTexture.get());
	return R_PASS;
}

RESULT DreamShareView::ShowCastingTexture() {
	m_pCastQuad->SetDiffuseTexture(m_pCastTexture.get());
	return R_PASS;
}

RESULT DreamShareView::SetCastingTexture(std::shared_ptr<texture> pNewCastTexture) {
	RESULT r = R_PASS;

	if (pNewCastTexture == nullptr) {
		CR(ShowLoadingTexture());
	}
	else {
		m_pCastTexture = pNewCastTexture;
	}

Error:
	return r;
}

RESULT DreamShareView::Show() {
	RESULT r = R_PASS;
	CR(GetComposite()->SetVisible(true));
Error:
	return r;
}

RESULT DreamShareView::Hide() {
	RESULT r = R_PASS;
	CR(GetComposite()->SetVisible(false));
Error:
	return r;
}

DreamAppHandle* DreamShareView::GetAppHandle() {
	return (DreamShareViewHandle*)(this);
}
