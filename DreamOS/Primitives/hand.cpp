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

RESULT hand::SetFrameOfReferenceObject(std::shared_ptr<DimObj> pParent, const hand::HandState& pHandState) {

	if (!CompareParent(pParent.get()) && pHandState.fOriented)
		pParent->AddChild(std::shared_ptr<DimObj>(this));
	return R_PASS;
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

	point ptModel = point(0.0f, 0.0f, 0.0f);
	float scaleModel = 0.015f;
	m_pLeftModel = AddModel(L"\\Models\\face4\\LeftHand.obj",
						nullptr,
						ptModel,
						scaleModel,
						vector((float)(M_PI_2), (float)(-M_PI_2), 0.0f));
	
	m_pRightModel = AddModel(L"\\Models\\face4\\RightHand.obj",
						nullptr,
						ptModel,
						scaleModel,
						vector((float)(M_PI_2), (float)(M_PI_2), 0.0f));

	m_qLeftModel = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, -(float)M_PI_2);
	m_qRightModel = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, (float)M_PI_2);
	
	m_fOriented = false;
	m_fSkeleton = false;

	m_qRotation = GetOrientation();

	m_fTracked = false;
	//Start all visibility at false
	OnLostTrack();

//Error:
	return r;
}

RESULT hand::SetOriented(bool fOriented) {
	m_fOriented = fOriented;
	return R_PASS;
}

bool hand::IsOriented() {
	return m_fOriented;
}

RESULT hand::SetSkeleton(bool fSkeleton) {
	m_fSkeleton = fSkeleton;
	return R_PASS;
}

bool hand::IsSkeleton() {
	return m_fSkeleton;
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
	m_pLeftModel->SetVisible(m_fTracked);
	m_pRightModel->SetVisible(m_fTracked);
	
	m_pPalm->SetVisible(m_fTracked);
	m_pIndexFinger->SetVisible(m_fTracked);
	m_pMiddleFinger->SetVisible(m_fTracked);
	m_pRingFinger->SetVisible(m_fTracked);
	m_pPinkyFinger->SetVisible(m_fTracked);
	m_pThumb->SetVisible(m_fTracked);
	return R_PASS;
}

RESULT hand::SetLocalOrientation(quaternion qRotation) {
	m_qRotation = qRotation;
	return R_PASS;
}

RESULT hand::SetFromLeapHand(const Leap::Hand hand) {
	RESULT r = R_PASS;

	m_fTracked = true;

	m_handType = (hand.isLeft()) ? HAND_LEFT : HAND_RIGHT;
	//m_leapHandID = hand.id();

	// update skeleton
	Leap::Vector leapPalmPosition = hand.palmPosition();
	leapPalmPosition /= 1000.0f;	// Leap outputs in mm, and our engine is in meters
	point ptPalmPosition = point(leapPalmPosition.x, leapPalmPosition.z, leapPalmPosition.y);

	SetPosition(ptPalmPosition * -1.0f);

	Leap::Matrix mBasis = hand.basis();
	Leap::Vector xAxis = mBasis.xBasis;
	Leap::Vector yAxis = mBasis.yBasis;
	Leap::Vector zAxis = mBasis.zBasis;

	vector vx = vector(-xAxis.x, -xAxis.z, -xAxis.y);
	vector vy = vector(-yAxis.x, -yAxis.z, -yAxis.y);
	vector vz = vector(-zAxis.x, -zAxis.z, -zAxis.y);
	
	// the x-axis is positive in the direction from the palm to the pinky,
	// so it must be reversed for the left hand
	if (hand.isLeft()) {
		vx = vector(xAxis.x, xAxis.z, xAxis.y);
	}

	quaternion qRotation = quaternion();
	qRotation.SetQuaternion(vx, vy, vz);
	qRotation.Reverse();

	m_qRotation = qRotation;

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

	// update model
	hand::HandType modelType = (m_fSkeleton) ? hand::HandType::HAND_SKELETON : m_handType;
	SetHandModel(modelType);
	m_pLeftModel->SetOrientation(m_qRotation * m_qLeftModel);
	m_pRightModel->SetOrientation(m_qRotation * m_qRightModel);
	
//Error:
	return r;
}

RESULT hand::SetHandModel(hand::HAND_TYPE type) {
	SetVisible();
	m_pLeftModel->SetVisible(type == hand::HAND_TYPE::HAND_LEFT);
	m_pRightModel->SetVisible(type == hand::HAND_TYPE::HAND_RIGHT);
	
	bool showSkeleton = type == hand::HAND_TYPE::HAND_SKELETON;
	m_pPalm->SetVisible(showSkeleton);
	m_pIndexFinger->SetVisible(showSkeleton);
	m_pMiddleFinger->SetVisible(showSkeleton);
	m_pRingFinger->SetVisible(showSkeleton);
	m_pPinkyFinger->SetVisible(showSkeleton);
	m_pThumb->SetVisible(showSkeleton);

	return R_PASS;
}

RESULT hand::ToggleRenderType() {
	m_fSkeleton = !m_fSkeleton;
	hand::HandType modelType = (m_fSkeleton) ? hand::HandType::HAND_SKELETON : m_handType;
	SetHandModel(modelType);

	return R_PASS;
}

RESULT hand::SetHandState(const hand::HandState& pHandState) {
	RESULT r = R_PASS;

	point pt = pHandState.ptPalm - point(0.0f, 0.0f, 0.25f);
	SetPosition(pt);
	//SetOrientation(pHandState.qOrientation);

	m_handType = pHandState.handType;
	hand::HandType modelType = (pHandState.fSkeleton) ? hand::HandType::HAND_SKELETON : m_handType;
	SetHandModel(modelType);

	m_fTracked = pHandState.fTracked;
	if (!m_fTracked)
		OnLostTrack();

	m_pIndexFinger->SetFingerState(pHandState.fingerIndex);
	m_pMiddleFinger->SetFingerState(pHandState.fingerMiddle);
	m_pRingFinger->SetFingerState(pHandState.fingerRing);
	m_pPinkyFinger->SetFingerState(pHandState.fingerPinky);
	m_pThumb->SetThumbState(pHandState.thumb);
	
	if (pHandState.fOriented) {
		m_pLeftModel->SetOrientation(pHandState.qOrientation * m_qLeftModel);
		m_pRightModel->SetOrientation(pHandState.qOrientation * m_qRightModel);
	}
	else {
		m_pLeftModel->SetOrientation(pHandState.qOrientation);
		m_pRightModel->SetOrientation(pHandState.qOrientation);
	}

//Error:
	return r;
}

hand::HandState hand::GetHandState() {
	hand::HandState handState = {
		m_handType,
		GetPosition(),
		m_qRotation,
		m_fOriented,
		m_fSkeleton,
		m_fTracked,
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
		false,
		false,
		finger::FingerState(),
		finger::FingerState(),
		finger::FingerState(),
		finger::FingerState(),
		thumb::ThumbState()
	};

	return handState;
}