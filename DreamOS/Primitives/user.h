#ifndef PRIMITIVE_USER_H_
#define PRIMITIVE_USER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/User/User.h
// User dimension object composite

#include "composite.h"
#include "hand.h"
#include "texture.h"

#include "DreamGarage/UICommon.h"

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

	typedef enum AvatarType {
		WOMAN = 1,
		BRUCE = 2,
		AVATAR_INVALID
	} AVATAR_TYPE;

public:
	user(HALImp* pHALImp);
	user(HALImp* pHALImp, DreamOS *pDreamOS);

	RESULT Initialize();
	std::shared_ptr<composite> GetHead();
	std::shared_ptr<model> GetMouth();
	std::shared_ptr<hand> GetHand(HAND_TYPE type);
	RESULT UpdateHand(const hand::HandState& pHandState);
	RESULT UpdateMouth(float mouthScale);	// used for scaling the user mouth
	
	RESULT Activate(user::ControllerType type = user::ControllerType::CONTROLLER_INVALID);

	RESULT SetDreamOS(DreamOS *pDreamOS);
	RESULT UpdateAvatarModelWithID(long avatarModelID);

	RESULT SetMouthPosition(point ptPosition);
	RESULT SetMouthOrientation(quaternion qOrientation);

	long GetAvatarModelId();

private:
	RESULT LoadHeadModelFromID();
	bool IsFemaleModel();

private:
	std::string k_strDefaultHeadPath = "\\Avatar_Woman\\avatar_1.FBX";
	std::string k_strMouthModelPath = "\\Avatars\\mouth.FBX";

	std::wstring k_wstrAvatarPath = L"Avatars\\avatar_";
	std::wstring k_wstrAvatarFileType = L".FBX";

	// TODO: will change with avatar specific mouths
	std::wstring k_wstrMouthMen = L"mouth_men/mouth_man_0";
	std::wstring k_wstrMouthWomen = L"mouth_women/mouth_women_0";
	std::wstring k_wstrMouthFileType = L".png";

private:
	// Storage of models and textures for head
	std::shared_ptr<model> m_pHead;
	std::vector<std::shared_ptr<texture>>m_pHeadTextures;

	std::shared_ptr<hand> m_pLeftHand;
	std::shared_ptr<hand> m_pRightHand;

	std::shared_ptr<model> m_pMouth = nullptr;
	std::shared_ptr<composite> m_pMouthComposite = nullptr;

	std::vector<std::shared_ptr<texture>> m_mouthStatesMen;
	std::vector<std::shared_ptr<texture>> m_mouthStatesWomen;

	// for adding mouths to UI shader 
	DreamOS *m_pDreamOS = nullptr;

private:

	long m_avatarModelId = AVATAR_INVALID;

	float m_mouthScale = 0.0f;
	int m_numMouthStates = 4;
	float m_headScale = HEAD_SCALE;
};

#endif	// ! PRIMITIVE_USER_H_