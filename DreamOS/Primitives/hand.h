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

#define HAND_ANIMATION_DURATION 0.25f

// overlay values
#define OVERLAY_ANIMATION_DURATION 0.5f
#define OVERLAY_VOLUME_WIDTH 0.15f // hands seem to be rotated in an unintuitive way
#define OVERLAY_VOLUME_HEIGHT 0.1f 
#define OVERLAY_VOLUME_DEPTH 0.15f

// oculus values
#define OVR_OVERLAY_SCALE 0.035f
#define OVR_OVERLAY_ASPECT_RATIO (332.0f / 671.0f)
#define OVR_OVERLAY_POSITION_X -0.575f
#define OVR_OVERLAY_POSITION_Y 0.0f
#define OVR_OVERLAY_POSITION_Z -0.575f

// vive values
#define VIVE_OVERLAY_SCALE 0.055f
#define VIVE_ASPECT_RATIO (541.0f / 599.0f)
#define VIVE_OVERLAY_POSITION_X -0.42f
#define VIVE_OVERLAY_POSITION_Y 0.15f
#define VIVE_OVERLAY_POSITION_Z 0.44f

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
	hand(HALImp* pHALImp, HAND_TYPE type, long avatarModelID);

	RESULT Initialize(HAND_TYPE type, long avatarModelID = -1);
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
	bool IsOverlayVisible();
	RESULT SetOverlayTexture(texture *pOverlayTexture);
	std::shared_ptr<volume> GetPhantomVolume();
	RESULT Update(); // TODO: app?
	RESULT SetVisible(bool fVisible = true, bool fSetChildren = true);

	RESULT PendCreateHandModel(long avatarModelID);
	RESULT LoadHandModel();

	// mallet replacement(?)
public:
	RESULT Show();
	RESULT Hide();
	std::shared_ptr<sphere> GetMalletHead();
	float GetMalletRadius();
	point GetMalletOffset();
	
private:
	float m_radius;
	float m_distance;
	float m_angle;
	std::shared_ptr<sphere> m_pHead;
	point m_headOffset;

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

	std::wstring k_wstrFolder = L"/avatar/";
	std::wstring k_wstrLeft = L"left-hand";
	std::wstring k_wstrRight = L"right-hand";
	std::wstring k_wstrFileType = L".fbx";

	HAND_TYPE m_handType;

	std::shared_ptr<composite> m_pModel = nullptr;;
	std::shared_ptr<sphere> m_pPalm = nullptr;

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
	// currently deprecated, and uninitialized
	std::shared_ptr<volume> m_pPhantomVolume = nullptr;
	float m_volumeWidth = OVERLAY_VOLUME_WIDTH;
	float m_volumeHeight = OVERLAY_VOLUME_HEIGHT;
	float m_volumeDepth = OVERLAY_VOLUME_DEPTH;

	ModelState m_modelState = ModelState::HAND;

	bool m_fLoadHandModel = false;
	long m_avatarModelId = -1;
};

#endif	// ! HAND_H_