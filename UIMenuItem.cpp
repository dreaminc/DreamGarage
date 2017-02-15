#include "UIMenuItem.h"

UIMenuItem::UIMenuItem(composite* c) :
m_pContext(c) 
{
	Initialize();
}

RESULT UIMenuItem::Initialize() {
	RESULT r = R_PASS;

	m_pQuad = m_pContext->AddQuad(1.0f, 1.0f);

	return r;
}

std::shared_ptr<composite> UIMenuItem::GetContext() {
	return m_pContext;
}

std::shared_ptr<quad> UIMenuItem::GetQuad() {
	return m_pQuad;
}

RESULT UIMenuItem::Update(IconFormat& iconFormat, LabelFormat& labelFormat) {
	RESULT r = R_PASS;

	// TODO: reuse this object?
	std::shared_ptr<FlatContext> pContext = m_pContext->MakeFlatContext();
	std::shared_ptr<text> pText;
	std::shared_ptr<quad> pIcon;
	CN(pContext);

	pText = pContext->AddText(
		labelFormat.font,
		labelFormat.label,
		labelFormat.fontSize,
		true // force distance fields
	);
	pText->MoveTo(labelFormat.position);

	pIcon = pContext->AddQuad(
		iconFormat.width,
		iconFormat.height,
		iconFormat.position
	);
	if (iconFormat.pTexture != nullptr)
		pIcon->SetColorTexture(iconFormat.pTexture.get());
	else
		pIcon->SetVisible(false);

	m_pContext->RenderToTexture(pContext);
	m_pQuad->SetColorTexture(pContext->GetFramebuffer()->GetTexture());

Error:
	return r;
}