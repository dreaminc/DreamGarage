#include "UIMallet.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationItem.h"

UIMallet::UIMallet(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	m_radius = 0.015f;
	m_pHead = m_pDreamOS->AddSphere(m_radius, 20.0f, 20.0f);
	m_pHead->SetVisible(false);
	m_headOffset = point(0.0f, MALLET_RADIUS * sin(MALLET_ANGLE), MALLET_RADIUS * cos(MALLET_RADIUS));
}

UIMallet::~UIMallet() {}

RESULT UIMallet::Show() {
	RESULT r = R_PASS;
	//TODO: Mallet animation
	m_pHead->SetVisible(true);
	
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pHead, 
		color(1.0f, 1.0f, 1.0f, 1.0f), 
		0.1, 
		AnimationCurveType::LINEAR, 
		AnimationFlags()));

Error:
	return r;
}

RESULT UIMallet::Hide() {
	RESULT r = R_PASS;
	m_pHead->SetVisible(false);

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pHead, 
		color(1.0f, 1.0f, 1.0f, 0.0f), 
		0.1, 
		AnimationCurveType::LINEAR, 
		AnimationFlags()));

Error:
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
