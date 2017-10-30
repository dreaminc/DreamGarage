#include "hand.h"
#include "Sense/SenseLeapMotionHand.h"
#include "DreamConsole/DreamConsole.h"
#include "Primitives/sphere.h"
#include "Primitives/model/model.h"

hand::hand(HALImp* pHALImp, HAND_TYPE type) :
	composite(pHALImp)
{
	Initialize(type);
}

RESULT hand::SetFrameOfReferenceObject(std::shared_ptr<DimObj> pParent, const hand::HandState& pHandState) {

	if (!CompareParent(pParent.get()) && pHandState.fOriented)
		pParent->AddChild(std::shared_ptr<DimObj>(this));

	return R_PASS;
}

std::shared_ptr<composite> hand::GetModel(HAND_TYPE handType) {
	return m_pModel;
}

RESULT hand::Initialize(HAND_TYPE type) {
	RESULT r = R_PASS;

	float palmRadius = 0.01f;
	point ptModel = point(0.0f, 0.0f, 0.0f);
	float scaleModel = 0.015f;

	m_pPalm = AddSphere(palmRadius, 10, 10);

	SetPosition(point(0.0f, 0.0f, -1.0f));

#ifndef _DEBUG
	if (type == HAND_TYPE::HAND_LEFT) {
		m_pModel = AddModel(L"\\face4\\LeftHand.obj");
		m_pModel->SetOrientationOffset((float)(-M_PI_2), (float)(M_PI_2), 0.0f);
	}
	
	if (type == HAND_TYPE::HAND_RIGHT) {
		m_pModel = AddModel(L"\\face4\\RightHand.obj");
		m_pModel->SetOrientationOffset((float)(-M_PI_2), (float)(-M_PI_2), 0.0f);
	}

	CN(m_pModel);

	m_pModel->SetPosition(ptModel);
	m_pModel->SetScale(scaleModel);
						
#else
	//m_pModel = AddComposite();
	//m_pModel->AddVolume(0.02f);
	m_pModel = AddModel(L"cube.obj");
	m_pModel->SetScale(0.02f);
#endif
	
	m_fOriented = false;

	m_qRotation = GetOrientation();

	m_fTracked = false;
	//Start all visibility at false
	CR(OnLostTrack());	//CR here because the only other C is inside of the #ifndef

Error:
	return r;
}

RESULT hand::SetOriented(bool fOriented) {
	m_fOriented = fOriented;
	return R_PASS;
}

bool hand::IsOriented() {
	return m_fOriented;
}

RESULT hand::SetTracked(bool fTracked) {
	m_fTracked = fTracked;
	return R_PASS;
}

bool hand::IsTracked() {
	return m_fTracked;
}

RESULT hand::OnLostTrack() {
	m_fTracked = false;
	
	m_pModel->SetVisible(m_fTracked);
	
	m_pPalm->SetVisible(m_fTracked);

	return R_PASS;
}

RESULT hand::SetLocalOrientation(quaternion qRotation) {
	m_qRotation = qRotation;
	return R_PASS;
}


RESULT hand::SetHandModel(HAND_TYPE type) {
	RESULT r = R_PASS;

	CBR(type == HAND_TYPE::HAND_SKELETON, R_SKIPPED) 

	SetVisible();
	m_pModel->SetVisible(true);

Error:
	return r;
}

RESULT hand::SetHandModelOrientation(quaternion qOrientation) {
	m_pModel->SetOrientation(qOrientation);
	return R_PASS;
}

RESULT hand::SetHandState(const hand::HandState& pHandState) {
	RESULT r = R_PASS;

	point pt = pHandState.ptPalm;
	SetPosition(pt);

	m_handType = pHandState.handType;
	SetHandModel(pHandState.handType);

	m_fTracked = pHandState.fTracked;
	if (!m_fTracked)
		OnLostTrack();

	m_pModel->SetOrientation(pHandState.qOrientation);

//Error:
	return r;
}

hand::HandState hand::GetHandState() {
	hand::HandState handState = {
		m_handType,
		GetPosition(true),
		m_qRotation,
		m_fOriented,
		m_fTracked
	};

	return handState;
}

hand::HandState hand::GetDebugHandState(HAND_TYPE handType) {
	hand::HandState handState = {
		handType,
		point(1,2,3),
		quaternion(),
		false,
		false
	};

	return handState;
}
