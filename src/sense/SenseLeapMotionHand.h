#ifndef SENSE_LEAPMOTION_HAND_H_
#define SENSE_LEAPMOTION_HAND_H_

#include <string>
#include <memory>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseLeapMotionHand.h
// Sense Leap Motion Hand Object
// This is a representation of the Sense Leap Motion Hand object that can then be 
// passed to the hand primitive in DreamOS

#include "Leap.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"

class hand;

typedef enum SenseLeapMotionHandType {
	SENSE_LEAPMOTION_HAND_LEFT,
	SENSE_LEAPMOTION_HAND_RIGHT,
	SENSE_LEAPMOTION_HAND_INVALID
} SENSE_LEAPMOTION_HAND_TYPE;

class SenseLeapMotionHand {
public:
	SenseLeapMotionHand(const Leap::Hand hand) {
		InitializeFromLeapHand(hand);
	}

	~SenseLeapMotionHand() {
		// empty
	}

	bool IsLeftHand() {
		return (m_handType == SENSE_LEAPMOTION_HAND_LEFT);
	}

	bool IsRightHand() {
		return (m_handType == SENSE_LEAPMOTION_HAND_RIGHT);
	}

	RESULT InitializeFromLeapHand(const Leap::Hand hand) {
		RESULT r = R_PASS;

		m_handType = (hand.isLeft()) ? SENSE_LEAPMOTION_HAND_LEFT : SENSE_LEAPMOTION_HAND_RIGHT;

		m_leapHandID = hand.id();
		Leap::Vector leapPalmPosition = hand.stabilizedPalmPosition();
		leapPalmPosition /= 1000.0f;	// Leap outputs in mm, and our engine is in meters

		m_ptPalmPosition = point(leapPalmPosition.x, leapPalmPosition.y, leapPalmPosition.z);

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

			Leap::Vector distalJointPosition = finger.jointPosition(Leap::Finger::JOINT_DIP);
			point ptDistalJointPosition = point(distalJointPosition.x, distalJointPosition.y, distalJointPosition.z);
			ptDistalJointPosition *= -(1.0f / 1000.0f);

			Leap::Vector proximalJointPosition = finger.jointPosition(Leap::Finger::JOINT_PIP);
			point ptProximalJointPosition = point(proximalJointPosition.x, proximalJointPosition.y, proximalJointPosition.z);
			ptProximalJointPosition *= -(1.0f / 1000.0f);

			Leap::Vector metacarpalJointPosition = finger.jointPosition(Leap::Finger::JOINT_MCP);
			point ptMetacarpalJointPosition = point(metacarpalJointPosition.x, metacarpalJointPosition.y, metacarpalJointPosition.z);
			ptMetacarpalJointPosition *= -(1.0f / 1000.0f);

			switch (finger.type()) {
			case Leap::Finger::TYPE_THUMB: {
				m_ptThumbFingerTipPosition = ptTipPosition;
				m_ptThumbFingerTipPosition = ptTipPosition;
				m_ptThumbFingerTipPosition = ptTipPosition;
				m_ptThumbFingerTipPosition = ptTipPosition;
			} break;

			case Leap::Finger::TYPE_INDEX: {
				m_ptIndexFingerTipPosition = ptTipPosition;
			} break;

			case Leap::Finger::TYPE_MIDDLE: {
				m_ptMiddleFingerTipPosition = ptTipPosition;
			} break;

			case Leap::Finger::TYPE_RING: {
				m_ptRingFingerTipPosition = ptTipPosition;
			} break;

			case Leap::Finger::TYPE_PINKY: {
				m_ptPinkyFingerTipPosition = ptTipPosition;
			} break;
			}
		}

		// joint positions

		/*
		Leap::Matrix handBasis = hand.basis();

		Leap::Vector leapPalmNormal = hand.palmNormal();
		Leap::Vector leapPalmDirection = hand.direction();
		vector vecPalmDir = vector(leapPalmDirection.x, leapPalmDirection.y, leapPalmDirection.z);
		vector vecPalmNorm = vector(leapPalmNormal.x, leapPalmNormal.y, leapPalmNormal.z);
		vector vecDir = vecPalmDir.NormalizedCross(vecPalmNorm);

		m_qOrientation = quaternion(vecPalmDir, vecPalmNorm, vecDir);
		*/


		/*
		m_qOrientation = quaternion(
		vector(leapPalmDirection.x, leapPalmDirection.y, leapPalmDirection.z),
		vector(leapPalmNormal.x, leapPalmNormal.y, leapPalmNormal.z)
		);
		*/

//	Error:
		return r;
	}

	const char *HandTypeString() {
		if (m_handType == SENSE_LEAPMOTION_HAND_LEFT)
			return "left";
		else if (m_handType == SENSE_LEAPMOTION_HAND_RIGHT)
			return "right";
		else
			return "invalid";
	}

	RESULT toString() {
		RESULT r = R_PASS;

		//DEBUG_LINEOUT("%s hand id:%d position:%s rol:%f", HandTypeString(), m_leapHandID, m_ptPalmPosition.toString().c_str());

//	Error:
		return r;
	}

	point PalmPosition() {
		return m_ptPalmPosition;
	}

	quaternion PalmOrientation() {
		return m_qOrientation;
	}

private:
	SENSE_LEAPMOTION_HAND_TYPE m_handType;
	quaternion m_qOrientation;
	int32_t m_leapHandID;

public:
	point m_ptPalmPosition;
	point m_ptThumbFingerTipPosition;
	point m_ptIndexFingerTipPosition;
	point m_ptMiddleFingerTipPosition;
	point m_ptRingFingerTipPosition;
	point m_ptPinkyFingerTipPosition;
};

#endif	// !SENSE_LEAPMOTION_HAND_H_