#ifndef PRIMITIVE_USER_H_
#define PRIMITIVE_USER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/User/User.h
// User dimension object composite

#include "composite.h"
#include "hand.h"
#include "texture.h"
#include "text.h"
#include "font.h"

#include "DreamGarage/UICommon.h"

#include <vector>

#define HEAD_POS 0.25f

// label
#define NAMETAG_BORDER 0.1f
#define NAMETAG_HEIGHT 0.05f
#define NAME_LINE_HEIGHT .12f
#define USERNAME_ANIMATION_DURATION 0.3f

#define BASE_LABEL_WIDTH 0.412f
#define LABEL_HEIGHT (BASE_LABEL_WIDTH) * (80.0f / 342.0f)
#define LABEL_PHOTO_WIDTH (LABEL_HEIGHT) // photo is square
#define LABEL_GAP_WIDTH (BASE_LABEL_WIDTH) * (30.0f / 342.0f)

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
	user(HALImp* pHALImp, DreamOS *pDreamOS);
	~user();

	RESULT Initialize();

	std::shared_ptr<composite> GetHead();
	std::shared_ptr<model> GetMouth();
	RESULT RemoveMouth();
	std::shared_ptr<hand> GetHand(HAND_TYPE type);

	RESULT Update();
	RESULT UpdateHand(const hand::HandState& pHandState);
	RESULT UpdateMouth(float mouthScale);	// used for scaling the user mouth
	
	RESULT SetDreamOS(DreamOS *pDreamOS);
	RESULT UpdateAvatarModelWithID(long avatarModelID);

	RESULT SetMouthPosition(point ptPosition);
	RESULT SetMouthOrientation(quaternion qOrientation);

	RESULT UpdateMouthPose();

	long GetAvatarModelId();
	int GetCurrentMouthPose();

	// label
public:
	// This needs to be called before InitializeUserNameLabel
	// because it uses the width of the rendered text object
	RESULT InitializeObject();
	RESULT InitializeUserNameText(std::string strScreenName);

	RESULT InitializeUserNameLabel();

	RESULT UpdateUserNameLabelPlacement(camera *pCamera);

	RESULT ShowUserNameField();
	RESULT HideUserNameField();

	RESULT SetUserLabelPosition(point ptPosition);
	RESULT SetUserLabelOrientation(quaternion qOrientation);

	bool HasProfilePhoto();
	RESULT SetProfilePhoto(std::string strProfilePhoto);
	RESULT SetScreenName(std::string strScreenName);
	RESULT SetInitials(std::string strInitials);
	std::string GetInitials();

	RESULT PendProfilePhotoDownload();
	RESULT OnProfilePhotoDownload(std::shared_ptr<std::vector<uint8_t>> pBufferVector, void* pContext);
	RESULT UpdateProfilePhoto();

	std::shared_ptr<composite> GetUserLabelComposite();
	std::shared_ptr<composite> GetUserObjectComposite();
	bool IsUserNameVisible();

private:
	std::wstring k_wstrLeft = L"UserLabel/user-label-background-left.png";
	std::wstring k_wstrMiddle = L"UserLabel/user-label-background-middle.png";
	std::wstring k_wstrRight = L"UserLabel/user-label-background-right.png";

	// Used instead of the left texture if the user does not have a profile picture
	std::wstring k_wstrLeftEmpty = L"UserLabel/user-label-background-left-empty.png";

	// Used for photo while download is pending
	std::wstring k_wstrPhoto = L"UserLabel/user-label-background-photo-temp.png";


private:
	color m_hiddenColor = color(1.0f, 1.0f, 1.0f, 0.0f);
	color m_backgroundColor = color(1.0f, 1.0f, 1.0f, 1.0f);
	color m_visibleColor = color(1.0f, 1.0f, 1.0f, 1.0f);

	std::shared_ptr<std::vector<uint8_t>> m_pPendingPhotoTextureBuffer;

	std::shared_ptr<composite> m_pUIObjectComposite = nullptr;
	std::shared_ptr<composite> m_pUserLabelComposite = nullptr;
	std::shared_ptr<text> m_pTextUserName = nullptr;
	std::shared_ptr<font> m_pFont = nullptr;

	std::shared_ptr<quad> m_pPhotoQuad = nullptr;
	std::shared_ptr<quad> m_pLeftGap = nullptr;
	std::shared_ptr<quad> m_pNameBackground = nullptr;
	std::shared_ptr<quad> m_pRightGap = nullptr;
	std::shared_ptr<texture> m_pTextBoxTexture = nullptr;

	// appear and disappear duration in seconds (direct plug into PushAnimation)
	float m_userNameAnimationDuration = USERNAME_ANIMATION_DURATION;

	std::string m_strScreenName;
	std::string m_strProfilePhotoURL;
	std::string m_strInitials;

	std::shared_ptr<sphere> m_pSphere = nullptr;

private:
	RESULT LoadHeadModelFromID();

private:

	std::wstring k_wstrAvatarPath = L"/avatar/";
	std::wstring k_wstrMouthFileType = L".png";

private:
	// Storage of models and textures for head
	std::shared_ptr<model> m_pHead;
	std::vector<std::shared_ptr<texture>>m_pHeadTextures;

	std::shared_ptr<hand> m_pLeftHand;
	std::shared_ptr<hand> m_pRightHand;

	std::shared_ptr<model> m_pMouth = nullptr;
	std::shared_ptr<composite> m_pMouthComposite = nullptr;

	std::vector<std::shared_ptr<texture>> m_mouthStates;
	std::vector<std::shared_ptr<texture>> m_mouthStatesWomen;

	// for adding mouths to UI shader 
	DreamOS *m_pDreamOS = nullptr;

private:

	long m_avatarModelId = -1;

	float m_mouthScale = 0.0f;
	int m_numMouthStates = 4;
	float m_headScale = HEAD_SCALE;
	int m_currentMouthPose = 0;
};

#endif	// ! PRIMITIVE_USER_H_