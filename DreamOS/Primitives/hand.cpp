#include "hand.h"
#include "Sense/SenseLeapMotionHand.h"
#include "DreamConsole/DreamConsole.h"


thumb::thumb(HALImp* pHALImp) :
	finger(pHALImp)
{
	//Initialize();
}

RESULT thumb::Initialize() {
	RESULT r = R_PASS;

	float width = 0.25f;
	float height = 0.25f;
	float length = 1.0f;
	float jointRadius = 0.01f;

	/*
	m_pMetacarpal = AddVolume(width, height, length);
	m_pMetacarpal->SetColor(color(COLOR_RED));
	m_pMetacarpal->UpdateBuffers();

	m_pProximalPhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
	m_pProximalPhalanx->SetPivotPoint(0, length, 0);
	m_pProximalPhalanx->SetColor(color(COLOR_BLUE));
	m_pProximalPhalanx->UpdateBuffers();
	m_pMetacarpal->AddChild(m_pProximalPhalanx);

	m_pDistalPhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
	m_pDistalPhalanx->SetPivotPoint(0, length, 0);
	m_pDistalPhalanx->SetColor(color(COLOR_YELLOW));
	m_pDistalPhalanx->UpdateBuffers();
	m_pProximalPhalanx->AddChild(m_pDistalPhalanx);
	*/

	m_pTip = AddSphere(jointRadius, 10, 10);
	m_pDIP = AddSphere(jointRadius, 10, 10);
	m_pPIP = AddSphere(jointRadius, 10, 10);
	//m_pMCP = AddSphere(jointRadius, 10, 10);

//Error:
	return r;
}

thumb::ThumbState thumb::GetThumbState() {
	ThumbState thumbState = {
		m_pTip->GetPosition(),
		m_pDIP->GetPosition(),
		m_pPIP->GetPosition()
	};

	return thumbState;
}

RESULT thumb::SetThumbState(const ThumbState& pThumbState) {
	RESULT r = R_PASS;

	m_pTip->SetPosition(pThumbState.ptTip);
	m_pDIP->SetPosition(pThumbState.ptDIP);
	m_pPIP->SetPosition(pThumbState.ptPIP);

//Error:
	return r;
}


finger::finger(HALImp* pHALImp) :
	composite(pHALImp),
	m_pMCP(nullptr),
	m_pTip(nullptr),
	m_pPIP(nullptr),
	m_pDIP(nullptr)
{
	//Initialize();
}

RESULT finger::Initialize() {
	RESULT r = R_PASS;

	float width = 0.25f;
	float height = 0.25f;
	float length = 1.0f;
	float jointRadius = 0.01f;

	/*
	m_pMetacarpal = AddVolume(width, height, length);
	m_pMetacarpal->SetColor(color(COLOR_RED));
	m_pMetacarpal->UpdateBuffers();

	m_pProximalPhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
	m_pProximalPhalanx->SetPivotPoint(0, length, 0);
	m_pProximalPhalanx->SetColor(color(COLOR_BLUE));
	m_pProximalPhalanx->UpdateBuffers();
	m_pMetacarpal->AddChild(m_pProximalPhalanx);

	m_pIntermediatePhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
	m_pIntermediatePhalanx->SetPivotPoint(0, length, 0);
	m_pIntermediatePhalanx->SetColor(color(COLOR_GREEN));
	m_pIntermediatePhalanx->UpdateBuffers();
	m_pProximalPhalanx->AddChild(m_pIntermediatePhalanx);

	m_pDistalPhalanx = MakeVolume(0.25f, 0.25f, 1.0f);
	m_pDistalPhalanx->SetPivotPoint(0, length, 0);
	m_pDistalPhalanx->SetColor(color(COLOR_YELLOW));
	m_pDistalPhalanx->UpdateBuffers();
	m_pIntermediatePhalanx->AddChild(m_pDistalPhalanx);
	*/

	m_pTip = AddSphere(jointRadius, 10, 10);
	m_pMCP = AddSphere(jointRadius, 10, 10);
	m_pDIP = AddSphere(jointRadius, 10, 10);
	m_pPIP = AddSphere(jointRadius, 10, 10);

//Error:
	return r;
}

finger::FingerState finger::GetFingerState() {
	FingerState fingerState = {
		m_pTip->GetPosition(),
		m_pMCP->GetPosition(),
		m_pDIP->GetPosition(),
		m_pPIP->GetPosition()
	};

	return fingerState;
}

RESULT finger::SetFingerState(const FingerState& pFingerState) {
	RESULT r = R_PASS;

	m_pTip->SetPosition(pFingerState.ptTip);
	m_pMCP->SetPosition(pFingerState.ptMCP);
	m_pDIP->SetPosition(pFingerState.ptDIP);
	m_pPIP->SetPosition(pFingerState.ptPIP);

//Error:
	return r;
}

RESULT finger::SetJointPosition(point ptJoint, JOINT_TYPE jointType) {
	switch (jointType) {
		case JOINT_MCP: {
			if (m_pMCP != nullptr)
				m_pMCP->SetPosition(ptJoint);
		} break;

		case JOINT_DIP: {
			if (m_pDIP != nullptr)
				m_pDIP->SetPosition(ptJoint);
		} break;

		case JOINT_PIP: {
			if (m_pPIP != nullptr)
				m_pPIP->SetPosition(ptJoint);
		} break;

		case JOINT_TIP: {
			if (m_pTip != nullptr)
				m_pTip->SetPosition(ptJoint);
		} break;
		
		default: 
		case JOINT_INVALID: {
			return R_FAIL;
		} break;
	}

	return R_PASS;
}


hand::hand(HALImp* pHALImp) :
	composite(pHALImp)
{
	Initialize();
}

RESULT hand::Initialize() {
	RESULT r = R_PASS;

	float palmRadius = 0.01f;

	m_pPalm = AddSphere(palmRadius, 10, 10);

	m_pIndexFinger = std::shared_ptr<finger>(new finger(m_pHALImp));
	m_pIndexFinger->Initialize();
	AddObject(m_pIndexFinger);

	m_pMiddleFinger = std::shared_ptr<finger>(new finger(m_pHALImp));
	m_pMiddleFinger->Initialize();
	AddObject(m_pMiddleFinger);

	m_pRingFinger = std::shared_ptr<finger>(new finger(m_pHALImp));
	m_pRingFinger->Initialize();
	AddObject(m_pRingFinger);

	m_pPinkyFinger = std::shared_ptr<finger>(new finger(m_pHALImp));
	m_pPinkyFinger->Initialize();
	AddObject(m_pPinkyFinger);

	m_pThumb = std::shared_ptr<thumb>(new thumb(m_pHALImp));
	m_pThumb->Initialize();
	AddObject(m_pThumb);

	SetPosition(point(0.0f, 0.0f, -1.0f));

	m_fOriented = false;

//Error:
	return r;
}

RESULT hand::SetOriented(bool attach) {
	m_fOriented = attach;
	return R_PASS;
}

bool hand::IsOriented() {
	return m_fOriented;
}

RESULT hand::SetFromLeapHand(const Leap::Hand hand) {
	RESULT r = R_PASS;

	m_handType = (hand.isLeft()) ? HAND_LEFT : HAND_RIGHT;
	//m_leapHandID = hand.id();

	Leap::Vector leapPalmPosition = hand.stabilizedPalmPosition();
	leapPalmPosition /= 1000.0f;	// Leap outputs in mm, and our engine is in meters
	point ptPalmPosition = point(leapPalmPosition.x, leapPalmPosition.z, leapPalmPosition.y);

	SetPosition(ptPalmPosition * -1.0f);

	// = hand.basis();
	Leap::Matrix handTransform;
	handTransform.origin = hand.palmPosition();
	handTransform = handTransform.rigidInverse();

	for (int i = 0; i < hand.fingers().count(); i++) {
		Leap::Finger leapFinger = hand.fingers()[i];
	
		Leap::Vector jointPosition;
		Leap::Vector transformedJointPosition;

		std::shared_ptr<finger> pFinger = nullptr;

		switch (leapFinger.type()) {
			case Leap::Finger::TYPE_THUMB: pFinger = m_pThumb; break;
			case Leap::Finger::TYPE_INDEX: pFinger = m_pIndexFinger; break;
			case Leap::Finger::TYPE_MIDDLE: pFinger = m_pMiddleFinger; break;
			case Leap::Finger::TYPE_RING: pFinger = m_pRingFinger; break;
			case Leap::Finger::TYPE_PINKY: pFinger = m_pPinkyFinger; break;
		}
	
		for (int j = 0; j <= (int)(Leap::Finger::JOINT_TIP); j++) {
			Leap::Finger::Joint jt = (Leap::Finger::Joint)(j);
			transformedJointPosition = handTransform.transformPoint(leapFinger.jointPosition(jt));
			point ptPosition = point(transformedJointPosition.x, transformedJointPosition.z, transformedJointPosition.y);
			ptPosition *= (-1.0f / 1000.0f);
			pFinger->SetJointPosition(ptPosition, (finger::JOINT_TYPE)(jt));
		}
	}

//Error:
	return r;
}

RESULT hand::SetHandType(hand::HAND_TYPE type) {
	m_handType = type;
	return R_PASS;
}

RESULT hand::SetHandState(const hand::HandState& pHandState) {
	RESULT r = R_PASS;

	point pt = pHandState.ptPalm - point(0.0f, 0.0f, 0.25f);
	SetPosition(pt);
	//SetOrientation(pHandState.qOrientation);

	m_pIndexFinger->SetFingerState(pHandState.fingerIndex);
	m_pMiddleFinger->SetFingerState(pHandState.fingerMiddle);
	m_pRingFinger->SetFingerState(pHandState.fingerRing);
	m_pPinkyFinger->SetFingerState(pHandState.fingerPinky);
	m_pThumb->SetThumbState(pHandState.thumb);

//Error:
	return r;
}

hand::HandState hand::GetHandState() {
	hand::HandState handState = {
		m_handType,
		GetPosition(),
		GetOrientation(),
		m_fOriented,
		m_pIndexFinger->GetFingerState(),
		m_pMiddleFinger->GetFingerState(),
		m_pRingFinger->GetFingerState(),
		m_pPinkyFinger->GetFingerState(),
		m_pThumb->GetThumbState()
	};

	return handState;
}

hand::HandState hand::GetDebugHandState(hand::HAND_TYPE handType) {
	hand::HandState handState = {
		handType,
		point(1,2,3),
		quaternion(),
		false,
		finger::FingerState(),
		finger::FingerState(),
		finger::FingerState(),
		finger::FingerState(),
		thumb::ThumbState()
	};

	return handState;
}
