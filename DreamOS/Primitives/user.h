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

#define HEAD_POS 0.25f

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
	std::shared_ptr<hand> GetHand(HAND_TYPE type);
	RESULT UpdateHand(const hand::HandState& pHandState);
	RESULT UpdateMouth(float mouthScale);	// used for scaling the user mouth
	
	RESULT Activate(user::ControllerType type = user::ControllerType::CONTROLLER_INVALID);

private:
		
	//std::string k_strDefaultHeadPath = "\\face4\\untitled.obj";
	//std::string k_strDefaultHeadPath = "\\head_01\\head_01.FBX";
	std::string k_strDefaultHeadPath = "\\Avatar 1\\avatar_1.FBX";
	// Storage of models and textures for head
	std::shared_ptr<model> m_pHead;
	std::vector<std::shared_ptr<texture>>m_pHeadTextures;

	std::shared_ptr<hand> m_pLeftHand;
	std::shared_ptr<hand> m_pRightHand;

	std::shared_ptr<quad> m_pMouth = nullptr;
	std::shared_ptr<texture> m_pMouthTexture = nullptr;
};

#endif	// ! PRIMITIVE_USER_H_