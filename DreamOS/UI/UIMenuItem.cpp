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

//	m_pContextComposite = m_pParentContext->AddComposite();
//	CN(m_pContextComposite);

	CR(m_pContextComposite->InitializeOBB());
	CR(m_pSurfaceComposite->InitializeOBB());

	//m_pQuad = m_pContextComposite->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f).Normal());
		//CR(pDreamOS->AddAndRegisterInteractionObject(m_pSurfaceComposite.get(), (InteractionEventType)(i), this));
	m_pSurface->SetMaterialAmbient(0.75f);
	m_strName = "";

	CR(InitializeOBB());

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

	labelFormat.pFont->SetLineHeight(0.025f);

	m_pLabel = std::shared_ptr<text>(m_pDreamOS->MakeText(
		labelFormat.pFont,
		labelFormat.strLabel, 
		0.225,
		0.0703125, 
		text::flags::WRAP | text::flags::TRAIL_ELLIPSIS | text::flags::RENDER_QUAD));

	m_pLabel->RotateXByDeg(90.0f);

	m_pLabel->SetPosition(labelFormat.ptPosition);

	vector bgNormal = vector(0.0f, sin(-M_PI / 6.0), cos(-M_PI / 6.0));
	auto pBgQuad = m_pSurfaceComposite->AddQuad(GetSurface()->GetWidth(), GetSurface()->GetHeight() / 2.0f, 1, 1, nullptr, bgNormal);

	point ptDiff = point(0.0f, (GetSurface()->GetHeight() + pBgQuad->GetHeight()) / 2.0f, 0.0001f);
	pBgQuad->SetPosition(GetSurface()->GetPosition() - ptDiff);
	pBgQuad->RotateXByDeg(-30.0f);

	if (labelFormat.pBgTexture != nullptr) {
		pBgQuad->SetDiffuseTexture(labelFormat.pBgTexture);
	}
	else {
		pBgQuad->SetColor(labelFormat.bgColor);
	}

	//AddObject(m_pLabel);
	m_pSurfaceComposite->AddObject(m_pLabel);

	m_strName = labelFormat.strLabel;

	if (iconFormat.pTexture != nullptr)
		m_pSurface->SetDiffuseTexture(iconFormat.pTexture);
	else
		m_pSurface->SetVisible(false);

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
	return m_pLabel->GetText();
}