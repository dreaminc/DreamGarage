#ifndef CLOUD_CONTROLLER_H_
#define CLOUD_CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/CloudController.h
// The base DreamCloud controller 

#include "CloudImp.h"
#include "User/UserFactory.h"
#include <memory>

class CloudController {
public:
	CloudController() :
		m_pCloudImp(nullptr),
		m_pUserController(nullptr)
	{
		// empty
	}

	~CloudController() {
		// empty 
	}

	RESULT SetCloudImp(std::unique_ptr<CloudImp> pCloudImp) {
		RESULT r = R_PASS;

		m_pCloudImp = std::move(pCloudImp);
		CN(m_pCloudImp);

	Error:
		return r;
	}

	RESULT InitializeUser(version ver = 1.0f) {
		RESULT r = R_PASS;

		m_pUserController = std::unique_ptr<User>(UserFactory::MakeUser(ver));
		CN(m_pUserController);

	Error:
		return r;
	}

	RESULT CreateNewURLRequest(std::wstring& strURL) {
		RESULT r = R_PASS;

		CNM(m_pCloudImp, "Cloud Imp not initialized");
		CRM(m_pCloudImp->CreateNewURLRequest(strURL), "Failed to create CEF URL request for %S", strURL.c_str());

	Error:
		return r;
	}

	RESULT LoginUser() {
		RESULT r = R_PASS;

		std::string strUsername = "dream@dreamos.com";
		std::string strPassword = "dreamy";

		// TODO: command line / config file - right now hard coded
		CN(m_pUserController);
		CR(m_pUserController->Login(strUsername, strPassword));

	Error:
		return r;
	}

	RESULT Update() {
		RESULT r = R_PASS;

		CR(m_pCloudImp->Update());

	Error:
		return r;
	}

private:
	UID m_uid;
	std::unique_ptr<CloudImp> m_pCloudImp;

	std::unique_ptr<User> m_pUserController;
};

#endif