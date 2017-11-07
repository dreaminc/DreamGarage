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

#define HAND_ANIMATION_DURATION 0.5
#define OVERLAY_ANIMATION_DURATION 0.5

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

	enum class ModelState {
		HAND,
		CONTROLLER,
		INVALID
	};

public:
	hand(HALImp* pHALImp, HAND_TYPE type);

	RESULT Initialize(HAND_TYPE type);
	RESULT InitializeWithContext(DreamOS *pDreamOS);

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

	RESULT SetModelState(ModelState modelState);
	ModelState GetModelState();
	RESULT SetOverlayVisible(bool fVisible);
	RESULT SetOverlayTexture(texture *pOverlayTexture);
	std::shared_ptr<volume> GetPhantomVolume();
	RESULT Update(); // TODO: app?

protected:
	//Animations
	//TODO: generalize
	RESULT ShowModel();
	RESULT HideModel();

	RESULT ShowController();
	RESULT HideController();

	RESULT ShowOverlay();
	RESULT HideOverlay();

protected:

	HAND_TYPE m_handType;

	std::shared_ptr<composite> m_pModel;
	std::shared_ptr<sphere> m_pPalm;

	bool m_fOriented;

	quaternion m_qRotation;

	// this is a state of the hand to represent whether the hand is tracked or not.
	// For example when using a leap motion, a hand is not tracked when it goes out of the sensor.
	bool	m_fTracked;


	// context for animations, HMD controllers, etc.
	DreamOS *m_pDreamOS = nullptr;

	// handle to HMD controller
	composite *m_pController = nullptr;

	std::shared_ptr<quad> m_pOverlayQuad = nullptr;
	bool m_fOverlayVisible = false;

	// collision volume
	std::shared_ptr<volume> m_pPhantomVolume = nullptr;

	ModelState m_modelState = ModelState::HAND;
};

#endif	// ! HAND_H_