#include "hand.h"
#include "Sense/SenseLeapMotionHand.h"


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

Error:
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

Error:
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

	float palmRadius = 0.02f;

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

Error:
	return r;
}

RESULT hand::SetFromLeapHand(const Leap::Hand hand) {
	RESULT r = R_PASS;

	m_handType = (hand.isLeft()) ? HAND_LEFT : HAND_RIGHT;
	//m_leapHandID = hand.id();

	Leap::Vector leapPalmPosition = hand.stabilizedPalmPosition();
	leapPalmPosition /= 1000.0f;	// Leap outputs in mm, and our engine is in meters
	point ptPalmPosition = point(leapPalmPosition.x, leapPalmPosition.y, leapPalmPosition.z);

	SetPosition(ptPalmPosition * -1.0f);

	Leap::Matrix handTransform = hand.basis();
	handTransform.origin = hand.palmPosition();
	handTransform = handTransform.rigidInverse();

	for (int i = 0; i < hand.fingers().count(); i++) {
		Leap::Finger finger = hand.fingers()[i];
		
		/*
		Leap::Vector transformedPosition = handTransform.transformPoint(finger.tipPosition());
		Leap::Vector transformedDirection = handTransform.transformDirection(finger.direction());
		*/
		// Do something with the transformed fingers

		// This will set it in reference to the palm
		Leap::Vector tipPosition = finger.jointPosition(Leap::Finger::JOINT_TIP);
		point ptTipPosition = point(tipPosition.x, tipPosition.y, tipPosition.z);
		ptTipPosition *= -(1.0f / 1000.0f);
		ptTipPosition -= ptPalmPosition;

		Leap::Vector distalJointPosition = finger.jointPosition(Leap::Finger::JOINT_DIP);
		point ptDistalJointPosition = point(distalJointPosition.x, distalJointPosition.y, distalJointPosition.z);
		ptDistalJointPosition *= -(1.0f / 1000.0f);
		ptDistalJointPosition -= ptPalmPosition;

		Leap::Vector proximalJointPosition = finger.jointPosition(Leap::Finger::JOINT_PIP);
		point ptProximalJointPosition = point(proximalJointPosition.x, proximalJointPosition.y, proximalJointPosition.z);
		ptProximalJointPosition *= -(1.0f / 1000.0f);
		ptProximalJointPosition -= ptPalmPosition;

		Leap::Vector metacarpalJointPosition = finger.jointPosition(Leap::Finger::JOINT_MCP);
		point ptMetacarpalJointPosition = point(metacarpalJointPosition.x, metacarpalJointPosition.y, metacarpalJointPosition.z);
		ptMetacarpalJointPosition *= -(1.0f / 1000.0f);
		ptMetacarpalJointPosition -= ptPalmPosition;

		switch (finger.type()) {
			case Leap::Finger::TYPE_THUMB: {
				m_pThumb->SetJointPosition(ptTipPosition, finger::JOINT_TYPE::JOINT_TIP);
				m_pThumb->SetJointPosition(ptDistalJointPosition, finger::JOINT_TYPE::JOINT_DIP);
				m_pThumb->SetJointPosition(ptProximalJointPosition, finger::JOINT_TYPE::JOINT_PIP);
				m_pThumb->SetJointPosition(ptMetacarpalJointPosition, finger::JOINT_TYPE::JOINT_MCP);
			} break;

			case Leap::Finger::TYPE_INDEX: {
				m_pIndexFinger->SetJointPosition(ptTipPosition, finger::JOINT_TYPE::JOINT_TIP);
				m_pIndexFinger->SetJointPosition(ptDistalJointPosition, finger::JOINT_TYPE::JOINT_DIP);
				m_pIndexFinger->SetJointPosition(ptProximalJointPosition, finger::JOINT_TYPE::JOINT_PIP);
				m_pIndexFinger->SetJointPosition(ptMetacarpalJointPosition, finger::JOINT_TYPE::JOINT_MCP);
			} break;

			case Leap::Finger::TYPE_MIDDLE: {
				m_pMiddleFinger->SetJointPosition(ptTipPosition, finger::JOINT_TYPE::JOINT_TIP);
				m_pMiddleFinger->SetJointPosition(ptDistalJointPosition, finger::JOINT_TYPE::JOINT_DIP);
				m_pMiddleFinger->SetJointPosition(ptProximalJointPosition, finger::JOINT_TYPE::JOINT_PIP);
				m_pMiddleFinger->SetJointPosition(ptMetacarpalJointPosition, finger::JOINT_TYPE::JOINT_MCP);
			} break;

			case Leap::Finger::TYPE_RING: {
				m_pRingFinger->SetJointPosition(ptTipPosition, finger::JOINT_TYPE::JOINT_TIP);
				m_pRingFinger->SetJointPosition(ptDistalJointPosition, finger::JOINT_TYPE::JOINT_DIP);
				m_pRingFinger->SetJointPosition(ptProximalJointPosition, finger::JOINT_TYPE::JOINT_PIP);
				m_pRingFinger->SetJointPosition(ptMetacarpalJointPosition, finger::JOINT_TYPE::JOINT_MCP);
			} break;

			case Leap::Finger::TYPE_PINKY: {
				m_pPinkyFinger->SetJointPosition(ptTipPosition, finger::JOINT_TYPE::JOINT_TIP);
				m_pPinkyFinger->SetJointPosition(ptDistalJointPosition, finger::JOINT_TYPE::JOINT_DIP);
				m_pPinkyFinger->SetJointPosition(ptProximalJointPosition, finger::JOINT_TYPE::JOINT_PIP);
				m_pPinkyFinger->SetJointPosition(ptMetacarpalJointPosition, finger::JOINT_TYPE::JOINT_MCP);
			} break;
		}
	}

Error:
	return r;
}
