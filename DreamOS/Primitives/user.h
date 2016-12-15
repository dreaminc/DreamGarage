#ifndef PRIMITIVE_USER_H_
#define PRIMITIVE_USER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/User/User.h
// User dimension object composite

#include "composite.h"
#include "hand.h"
#include "texture.h"

#include <vector>

class user : public composite {
public:
	typedef enum ControllerType {
		LEAP_SKELETON,
		LEAP_MODEL,
		VIVE_MODEL,
		CONTROLLER_INVALID
	} CONTROLLER_TYPE;

public:
	user(HALImp* pHALImp);

	RESULT Initialize();
	std::shared_ptr<composite> GetHead();
	RESULT UpdateHand(const hand::HandState& pHandState);
	RESULT UpdateMouth(float mouthScale);	// used for scaling the user mouth
	RESULT SwitchHeadModel();
	RESULT Activate(user::ControllerType type = user::ControllerType::CONTROLLER_INVALID);
//	std::shared_ptr<composite> GetHandModel(hand::HAND_TYPE type);
//	std::shared_ptr<composite> GetViveModel(hand::HAND_TYPE type);

private:
	//std::shared_ptr<composite> m_pHead;
	//std::shared_ptr<texture> m_pTextureHead;

	// Storage of models and textures for head
	std::vector<std::shared_ptr<composite>> m_pHeads;
	std::shared_ptr<composite> m_pHead;
	std::vector<std::shared_ptr<texture>>m_pHeadTextures;

	std::shared_ptr<hand> m_pLeapLeftHand;
	std::shared_ptr<hand> m_pLeapRightHand;
	/*
	std::shared_ptr<composite> m_pLeapLeftModel;
	std::shared_ptr<composite> m_pLeapRightModel;

	std::shared_ptr<composite> m_pViveLeftModel;
	std::shared_ptr<composite> m_pViveRightModel;
//*/

	std::shared_ptr<quad> m_pMouth = nullptr;
	std::shared_ptr<texture> m_pMouthTexture = nullptr;
	bool stupidLeft = false;
	bool stupidRight = false;
};

#endif	// ! PRIMITIVE_USER_H_