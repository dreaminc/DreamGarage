#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/Controller.h
// The base controller object

#include <memory>

class Controller {
public:
	Controller() :
		m_pParentController(nullptr)
	{
		// empty
	}

	Controller(Controller *pParentController) :
		m_pParentController(pParentController)
	{
		// empty
	}

	virtual ~Controller() {}

protected:
	Controller* GetParentController() { return m_pParentController; }

private:
	Controller* m_pParentController;

private:
	UID m_uid;
};

#endif	// !CONTROLLER_H_