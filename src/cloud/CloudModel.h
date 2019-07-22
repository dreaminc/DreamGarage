#ifndef CLOUD_MODEL_H_
#define CLOUD_MODEL_H_

#include "core/ehm/EHM.h"

// Dream Cloud
// dos/src/cloud/CloudModel.h

// The base controller object

#include <memory>

#include "core/types/Controller.h"

#include "core/types/UID.h"
#include "core/types/valid.h"

class CloudModel {
public:
	CloudModel() :
		m_pParentController(nullptr)
	{
		// empty
	}

	CloudModel(std::shared_ptr<Controller> pParentController) :
		m_pParentController(std::shared_ptr<Controller>(pParentController))
	{
		// empty
	}

	std::shared_ptr<Controller> GetParentController() { return m_pParentController; }

private:
	UID m_uid;
	std::shared_ptr<Controller> m_pParentController;
};

#endif	// ! CLOUD_MODEL_H_