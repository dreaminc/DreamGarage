#include "UIMallet.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationItem.h"

UIMallet::UIMallet(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	Initialize();
}

UIMallet::~UIMallet() {}

RESULT UIMallet::Initialize() {

	// default
	m_radius = MALLET_RADIUS;

	// TODO: move to hmd setting mallet offsets
	auto pHMD = m_pDreamOS->GetHMD();
	if (pHMD != nullptr) {
		switch (pHMD->GetDeviceType()) {
			case HMDDeviceType::OCULUS: {
				m_distance = 0.2f;
				m_angle = -23.0f * (float)(M_PI) / 180.0f;

			} break;
			case HMDDeviceType::VIVE: {
				m_distance = 0.16f;
				m_angle = -20.0f * (float)(M_PI) / 180.0f;
			} break;
			case HMDDeviceType::META: {
				m_distance = 0.0f;
				m_angle = 0.0f;
			} break;
		}
	}
	m_headOffset = point(0.0f, m_distance * sin(m_angle), -m_distance * cos(m_angle));

	m_pHead = m_pDreamOS->AddSphere(m_radius, 20.0f, 20.0f);
	m_pHead->SetVisible(false);

	return R_PASS;
}

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
