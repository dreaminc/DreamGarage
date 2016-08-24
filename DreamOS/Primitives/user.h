#ifndef PRIMITIVE_USER_H_
#define PRIMITIVE_USER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/User/User.h
// User dimension object composite

#include "composite.h"
#include "hand.h"
#include "texture.h"

class user : public composite {
public:
	user(HALImp* pHALImp);

	RESULT Initialize();

	RESULT UpdateHand(const hand::HandState& pHandState);

private:
	std::shared_ptr<composite> m_pHead;
	std::shared_ptr<texture> m_pTextureHead;

	std::shared_ptr<hand> m_pLeftHand;
	std::shared_ptr<hand> m_pRightHand;
};

#endif	// ! PRIMITIVE_USER_H_