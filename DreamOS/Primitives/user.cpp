#include "user.h"

user::user(HALImp* pHALImp) :
	composite(pHALImp)
{
	Initialize();
}

RESULT user::Initialize() {
	RESULT r = R_PASS;
	
	// TODO: Make this programatic 

	m_pTextureHead = MakeTexture(L"..\\Models\\face2\\faceP.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	m_pHead = AddModel(L"\\Models\\face2\\untitled.obj",
					   m_pTextureHead.get(),
					   point(0.0f, 0.0f, 0.0f),
					   0.05f,
					   (point_precision)(0.0f));

	
	m_pLeftHand = AddHand();
	m_pRightHand = AddHand();

	SetPosition(point(0.0f, 0.0f, 0.0f));

	//Error:
	return r;
}