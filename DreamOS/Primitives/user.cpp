#include "user.h"

user::user(HALImp* pHALImp) :
	composite(pHALImp),
	m_pTextureHead(nullptr),
	m_pHead(nullptr)
{
	Initialize();
}

RESULT user::Initialize() {
	RESULT r = R_PASS;
	
	// TODO: Make this programatic 

	///*

	///*
	m_pTextureHead = MakeTexture(L"..\\Models\\face2\\faceP.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	m_pHead = AddModel(L"\\Models\\face2\\untitled.obj",
					   m_pTextureHead.get(),
					   point(0.0f, 0.0f, 0.0f),
					   0.02f,
					   (point_precision)(0.0f));
	//*/

	/*
	m_pTextureHead = MakeTexture(L"..\\Models\\face2\\faceP.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	m_pHead = AddModel(L"\\Models\\stormtrooper\\stormtrooper.obj",
						nullptr,
						point(0.0f, 0.0f, 0.0f),
						0.02f,
						(point_precision)(0.0f));
	//*/
	
	m_pLeftHand = AddHand();
	m_pRightHand = AddHand();

	SetPosition(point(0.0f, 0.0f, 0.0f));

	//Error:
	return r;
}

RESULT user::UpdateHand(const hand::HandState& pHandState) {
	RESULT r = R_PASS;

	if (pHandState.handType == hand::HAND_LEFT) {
		m_pLeftHand->SetHandState(pHandState);
	}
	else if (pHandState.handType == hand::HAND_RIGHT) {
		m_pRightHand->SetHandState(pHandState);
	}

//Error:
	return r;
}