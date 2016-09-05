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
	user(HALImp* pHALImp);

	RESULT Initialize();
	RESULT UpdateHand(const hand::HandState& pHandState);
	RESULT SwitchHeadModel();

private:
	//std::shared_ptr<composite> m_pHead;
	//std::shared_ptr<texture> m_pTextureHead;

	// Storage of models and textures for head
	std::vector<std::shared_ptr<composite>> m_pHeads;
	std::vector<std::shared_ptr<texture>>m_pHeadTextures;

	std::shared_ptr<hand> m_pLeftHand;
	std::shared_ptr<hand> m_pRightHand;
};

#endif	// ! PRIMITIVE_USER_H_