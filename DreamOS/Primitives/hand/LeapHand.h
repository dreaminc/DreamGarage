#ifndef LEAP_HAND_H_
#define LEAP_HAND_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/hand/LeapHand.h

#include "Leap.h"

#include "hand.h"

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

class LeapHand : public hand 
{
public:
	struct LeapHandState : HandState {

		finger::FingerState fingerIndex;
		finger::FingerState fingerMiddle;
		finger::FingerState fingerRing;
		finger::FingerState fingerPinky;
		thumb::ThumbState thumb;

		bool fSkeleton;

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
	LeapHand(HALImp* pHALImp, HAND_TYPE type);

	RESULT Initialize(HAND_TYPE type);

	RESULT SetFromLeapHand(const Leap::Hand hand);

	RESULT OnLostTrack() override;

	RESULT SetSkeleton(bool fSkeleton);
	bool IsSkeleton();
	RESULT ToggleRenderType();

	HandState GetHandState() override;
	RESULT SetHandState(const LeapHand::LeapHandState& pHandState);

	RESULT SetHandModel(HAND_TYPE type) override;

private:

	std::shared_ptr<finger> m_pIndexFinger;
	std::shared_ptr<finger> m_pMiddleFinger;
	std::shared_ptr<finger> m_pRingFinger;
	std::shared_ptr<finger> m_pPinkyFinger;

	std::shared_ptr<thumb> m_pThumb;

	bool m_fSkeleton;
};

#endif