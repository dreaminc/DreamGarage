#include "UIMenuItem.h"

UIMenuItem::UIMenuItem(composite* c) :
m_context(c) 
{
	Initialize();
}

UIMenuItem::~UIMenuItem() {
	// empty
}

RESULT UIMenuItem::Initialize() {
	RESULT r = R_PASS;

	m_pButton = m_context->AddComposite();
	m_pQuad = m_pButton->AddQuad(1.0f, 1.0f);

	return r;
}

std::shared_ptr<composite> UIMenuItem::GetButton() {
	return m_pButton;
}

std::shared_ptr<quad> UIMenuItem::GetQuad() {
	return m_pQuad;
}

RESULT UIMenuItem::Update(IconFormat iconFormat, LabelFormat labelFormat) {
	RESULT r = R_PASS;

	// TODO: reuse this object?
	std::shared_ptr<FlatContext> pContext = m_context->MakeFlatContext();

	std::shared_ptr<text> pText = pContext->AddText(
		labelFormat.font,
		labelFormat.label,
		labelFormat.fontSize,
		true // force distance fields
	);
	pText->MoveTo(labelFormat.position);

	std::shared_ptr<quad> pIcon = pContext->AddQuad(
		iconFormat.width,
		iconFormat.height,
		iconFormat.position
	);
	if (iconFormat.pTexture != nullptr)
		pIcon->SetColorTexture(iconFormat.pTexture.get());
	else
		pIcon->SetVisible(false);

	m_context->RenderToTexture(pContext);
	m_pQuad->SetColorTexture(pContext->GetFramebuffer()->GetTexture());

	return r;
}