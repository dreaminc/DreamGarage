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

#include <memory>

class SenseLeapMotionHand;
class model;

class hand : public composite {

public:
	struct HandState {
		HAND_TYPE handType;
		point ptPalm;
		quaternion qOrientation;

		bool fOriented;
		bool fTracked;

		RESULT PrintState() {
			ptPalm.Print();
			return R_PASS;
		}
	};

public:
	hand(HALImp* pHALImp, HAND_TYPE type);

	RESULT Initialize(HAND_TYPE type);

	//RESULT SetFromLeapMotionHand(SenseLeapMotionHand sHand);
	virtual RESULT OnLostTrack();
	RESULT SetHandState(const hand::HandState& pHandState);

	RESULT SetOriented(bool fOriented);
	bool IsOriented();
	RESULT SetTracked(bool fTracked);
	bool IsTracked();
	RESULT SetLocalOrientation(quaternion qRotation);

	virtual hand::HandState GetHandState();
	static hand::HandState GetDebugHandState(HAND_TYPE handType);
	RESULT SetFrameOfReferenceObject(std::shared_ptr<DimObj> pParent, const hand::HandState& pHandState);
	virtual RESULT SetHandModel(HAND_TYPE type);
	RESULT SetHandModelOrientation(quaternion qOrientation);

	std::shared_ptr<composite> GetModel(HAND_TYPE handType);

protected:

	HAND_TYPE m_handType;

	std::shared_ptr<composite> m_pModel;
	std::shared_ptr<sphere> m_pPalm;

	bool m_fOriented;

	quaternion m_qRotation;

	// this is a state of the hand to represent whether the hand is tracked or not.
	// For example when using a leap motion, a hand is not tracked when it goes out of the sensor.
	bool	m_fTracked;
};

#endif	// ! HAND_H_