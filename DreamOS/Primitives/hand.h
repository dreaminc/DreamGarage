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
#include "HandType.h"

class SenseLeapMotionHand;
class model;

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

	struct FingerState {
		point ptTip;
		point ptMCP;
		point ptPIP;
		point ptDIP;

		RESULT PrintState() {
			DEBUG_LINEOUT("finger state:");
			ptTip.Print();
			ptMCP.Print();
			ptPIP.Print();
			ptDIP.Print();

			return R_PASS;
		}
	};

public:
	finger(HALImp* pHALImp);

	RESULT Initialize();
	RESULT SetJointPosition(point ptJoint, JOINT_TYPE jointType);
	RESULT SetFingerState(const FingerState& pFingerState);
	finger::FingerState GetFingerState();

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
	struct ThumbState {
		point ptTip;
		point ptPIP;
		point ptDIP;

		RESULT PrintState() {
			DEBUG_LINEOUT("thumb state:");
			ptTip.Print();
			ptPIP.Print();
			ptDIP.Print();

			return R_PASS;
		}
	};

public:
	thumb(HALImp* pHALImp);

	RESULT Initialize();
	RESULT SetThumbState(const ThumbState& pThumbState);
	thumb::ThumbState GetThumbState();
};

class hand : public composite {

public:
	struct HandState {
		HAND_TYPE handType;
		point ptPalm;
		quaternion qOrientation;

		bool fOriented;
		bool fSkeleton;
		bool fTracked;

		finger::FingerState fingerIndex;
		finger::FingerState fingerMiddle;
		finger::FingerState fingerRing;
		finger::FingerState fingerPinky;
		thumb::ThumbState thumb;

		RESULT PrintState() {
			ptPalm.Print();
			fingerIndex.PrintState();
			fingerMiddle.PrintState();
			fingerRing.PrintState();
			fingerPinky.PrintState();
			thumb.PrintState();

			return R_PASS;
		}
	};

public:
	hand(HALImp* pHALImp, HAND_TYPE type);

	RESULT Initialize(HAND_TYPE type);

	//RESULT SetFromLeapMotionHand(SenseLeapMotionHand sHand);
	RESULT SetFromLeapHand(const Leap::Hand hand);
	RESULT OnLostTrack();
	RESULT SetHandState(const hand::HandState& pHandState);

	RESULT SetOriented(bool fOriented);
	bool IsOriented();
	RESULT SetSkeleton(bool fSkeleton);
	bool IsSkeleton();
	RESULT SetTracked(bool fTracked);
	bool IsTracked();
	RESULT SetLocalOrientation(quaternion qRotation);

	hand::HandState GetHandState();
	static hand::HandState GetDebugHandState(HAND_TYPE handType);
	RESULT ToggleRenderType();
	RESULT SetFrameOfReferenceObject(std::shared_ptr<DimObj> pParent, const hand::HandState& pHandState);
	RESULT SetHandModel(HAND_TYPE type);
	RESULT SetHandModelOrientation(quaternion qOrientation);

	std::shared_ptr<composite> GetModel(HAND_TYPE handType);

private:

	HAND_TYPE m_handType;

	std::shared_ptr<composite> m_pModel;
	std::shared_ptr<sphere> m_pPalm;

	std::shared_ptr<finger> m_pIndexFinger;
	std::shared_ptr<finger> m_pMiddleFinger;
	std::shared_ptr<finger> m_pRingFinger;
	std::shared_ptr<finger> m_pPinkyFinger;

	std::shared_ptr<thumb> m_pThumb;

	bool m_fOriented;
	bool m_fSkeleton;

	quaternion m_qRotation;

	// this is a state of the hand to represent whether the hand is tracked or not.
	// For example when using a leap motion, a hand is not tracked when it goes out of the sensor.
	bool	m_fTracked;
};

#endif	// ! HAND_H_