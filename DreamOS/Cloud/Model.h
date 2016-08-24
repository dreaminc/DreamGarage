#ifndef MODEL_H_
#define MODEL_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/Model.h
// The base controller object

#include "Controller.h"
#include <memory>

class Model {
public:
	Model() :
		m_pParentController(nullptr)
	{
		// empty
	}

	Model(std::shared_ptr<Controller> pParentController) :
		m_pParentController(std::shared_ptr<Controller>(pParentController))
	{
		// empty
	}

	std::shared_ptr<Controller> GetParentController() { return m_pParentController; }

private:
	UID m_uid;
	std::shared_ptr<Controller> m_pParentController;
};

#endif	// ! MODEL_H_