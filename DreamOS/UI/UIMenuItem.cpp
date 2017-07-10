#include "UIMenuItem.h"
#include "Primitives/font.h"
#include "Primitives/text.h"
#include "Primitives/framebuffer.h"
#include "DreamOS.h"

UIMenuItem::UIMenuItem(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIButton(pHALImp, pDreamOS)
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

	//m_pQuad = m_pContextComposite->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f).Normal());
	m_pSurface->SetMaterialAmbient(0.75f);
	m_strName = "";

Error:
	return r;
}

std::shared_ptr<composite> UIMenuItem::GetContext() {
	return m_pContextComposite;
}

std::shared_ptr<quad> UIMenuItem::GetQuad() {
	return m_pSurface;
}

RESULT UIMenuItem::Update(IconFormat& iconFormat, LabelFormat& labelFormat) {
	RESULT r = R_PASS;

	std::shared_ptr<text> pText;
	std::shared_ptr<quad> pIcon;

	pText = std::shared_ptr<text>(m_pDreamOS->MakeText(labelFormat.pFont,
		labelFormat.strLabel, 0.25, text::flags::FIT_TO_SIZE | text::flags::RENDER_QUAD));
	pText->RotateXByDeg(90.0f);

	pText->SetPosition(labelFormat.ptPosition);
	AddObject(pText);

	m_strName = labelFormat.strLabel;

	pIcon = AddQuad(1.0f, 1.0f);
	pIcon->RotateXByDeg(90.0f);

	pIcon->SetPosition(iconFormat.ptPosition);
	if (iconFormat.pTexture != nullptr)
		pIcon->SetColorTexture(iconFormat.pTexture);
	else
		pIcon->SetVisible(false);

//Error:
	return r;
}

RESULT UIMenuItem::SetObjectParams(point ptQuad, quaternion qQuad, point ptContext, quaternion qContext) {
	m_pSurface->MoveTo(ptQuad);
	m_pSurface->SetOrientation(qQuad);

	m_pContextComposite->MoveTo(ptContext);
	m_pContextComposite->SetOrientation(qContext);

	return R_PASS;
}

// TODO: could be different for different types of MenuItems
// may make sense to take a comparison function as an argument
bool UIMenuItem::Contains(VirtualObj *pObj) {
	return (pObj == m_pSurface.get());
}

std::string& UIMenuItem::GetName() {
	return m_strName;
}