#ifndef HAND_H_
#define HAND_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/composite.h
// Composite Primitive
// The composite object is a collection of other objects when these are constructed within the object itself 
// this is really a convenience object that should be used to derive others rather than off of DimObj directly

#include "composite.h"
#include "Leap.h"

class SenseLeapMotionHand;

class finger : public composite {
public:
	// Note: This is compatible with the leap motion library
	typedef enum JointType {
		JOINT_MCP,		// Metacarpal phalangeal Joint
		JOINT_PIP,		// Proximal interphalangeal joint
		JOINT_DIP,		// distal interphalangeal joint
		JOINT_TIP,		// tip joint
		JOINT_INVALID
	} JOINT_TYPE;

public:
	finger(HALImp* pHALImp);

	RESULT Initialize();
	RESULT SetJointPosition(point ptJoint, JOINT_TYPE jointType);

protected:
	/*
	std::shared_ptr<volume> m_pMetacarpal;
	std::shared_ptr<volume> m_pProximalPhalanx;
	std::shared_ptr<volume> m_pIntermediatePhalanx;
	std::shared_ptr<volume> m_pDistalPhalanx;
	*/

	std::shared_ptr<sphere> m_pTip;
	std::shared_ptr<sphere> m_pMCP;
	std::shared_ptr<sphere> m_pPIP;
	std::shared_ptr<sphere> m_pDIP;
};

class thumb : public finger {
public:
	thumb(HALImp* pHALImp);

	RESULT Initialize();
};

class hand : public composite {
public:
	typedef enum HandType {
		HAND_LEFT,
		HAND_RIGHT,
		HAND_INVALID
	} HAND_TYPE;

public:
	hand(HALImp* pHALImp);

	RESULT Initialize();

	//RESULT SetFromLeapMotionHand(SenseLeapMotionHand sHand);
	RESULT SetFromLeapHand(const Leap::Hand hand);

private:
	HAND_TYPE m_handType;

	std::shared_ptr<sphere> m_pPalm;

	std::shared_ptr<finger> m_pIndexFinger;
	std::shared_ptr<finger> m_pMiddleFinger;
	std::shared_ptr<finger> m_pRingFinger;
	std::shared_ptr<finger> m_pPinkyFinger;

	std::shared_ptr<thumb> m_pThumb;
};

#endif	// ! HAND_H_