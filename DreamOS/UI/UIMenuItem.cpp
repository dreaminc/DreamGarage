#include "UIMenuItem.h"

UIMenuItem::UIMenuItem(std::shared_ptr<composite> pParentComposite) :
	m_pParentContext(pParentComposite),
	m_pContextComposite(nullptr)
{
	RESULT r = R_PASS;

	CR(Initialize());

// Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

RESULT UIMenuItem::Initialize() {
	RESULT r = R_PASS;

	CN(m_pParentContext);
	m_pContextComposite = m_pParentContext->AddComposite();
	CN(m_pContextComposite);

	CR(m_pContextComposite->InitializeOBB());

	m_pQuad = m_pContextComposite->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f).Normal());
	m_strName = "";

Error:
	return r;
}

std::shared_ptr<composite> UIMenuItem::GetContext() {
	return m_pContextComposite;
}

std::shared_ptr<quad> UIMenuItem::GetQuad() {
	return m_pQuad;
}

RESULT UIMenuItem::Update(IconFormat& iconFormat, LabelFormat& labelFormat) {
	RESULT r = R_PASS;

	// TODO: reuse this object?
	std::shared_ptr<FlatContext> pContext = m_pContextComposite->MakeFlatContext();
	std::shared_ptr<text> pText;
	std::shared_ptr<quad> pIcon;
	CN(pContext);

	pText = pContext->AddText(
		labelFormat.font,
		labelFormat.strLabel,
		labelFormat.fontSize,
		true // force distance fields
	);
	pText->MoveTo(labelFormat.ptPosition);

	m_strName = labelFormat.strLabel;

	pIcon = pContext->AddQuad(
		iconFormat.width,
		iconFormat.height,
		iconFormat.ptPosition
	);
	if (iconFormat.pTexture != nullptr)
		pIcon->SetColorTexture(iconFormat.pTexture.get());
	else
		pIcon->SetVisible(false);

	m_pContextComposite->RenderToTexture(pContext);
	m_pQuad->SetColorTexture(pContext->GetFramebuffer()->GetTexture());

Error:
	return r;
}

std::string& UIMenuItem::GetName() {
	return m_strName;
}