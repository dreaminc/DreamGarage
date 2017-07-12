#include "UIMallet.h"
#include "DreamOS.h"

UIMallet::UIMallet(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	m_radius = 0.02f;
	m_pHead = m_pDreamOS->AddSphere(m_radius, 20.0f, 20.0f);
	m_pHead->SetVisible(false);
	m_headOffset = point(0.0f, 0.0f, -0.2f);
}

UIMallet::~UIMallet() {}

RESULT UIMallet::Show() {
	RESULT r = R_PASS;
	//TODO: Mallet animation
	m_pHead->SetVisible(true);
//Error:
	return r;
}

RESULT UIMallet::Hide() {
	RESULT r = R_PASS;
	m_pHead->SetVisible(false);
//Error:
	return r;
}

float UIMallet::GetRadius() {
	return m_radius;
}

sphere *UIMallet::GetMalletHead() {
	return m_pHead;
}

point UIMallet::GetHeadOffset() {
	return m_headOffset;
}