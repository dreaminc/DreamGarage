#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/Controller.h
// The base controller object

#include <memory>

class CloudController;

class Controller {
public:
	Controller(Controller *pParentController = nullptr, CloudController *pParentCloudController = nullptr) :
		m_pParentController(pParentController),
		m_pParentCloudController(pParentCloudController)
	{
		// empty
	}

	virtual ~Controller() {}

	virtual RESULT Initialize() = 0;

	friend class Controller;

protected:
	Controller* GetParentController() { 
		return m_pParentController; 
	}

	CloudController *GetCloudController() {
		if (m_pParentCloudController != nullptr) {
			return m_pParentCloudController;
		}
		else if (m_pParentController != nullptr) {
			return m_pParentController->GetCloudController();
		}
		else {
			return nullptr;
		}
	}

private:
	Controller* m_pParentController;
	CloudController *m_pParentCloudController;

private:
	UID m_uid;
};

#endif	// !CONTROLLER_H_