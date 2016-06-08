#ifndef CLOUD_IMP_H_
#define CLOUD_IMP_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

// DREAM OS
// DreamOS/Cloud/CloudImp.h
// The base DreamCloud Implementation intefrace

class CloudImp {
public:
	CloudImp() {
		// empty
	}

	~CloudImp() {
		// empty
	}

private:
	UID m_uid;
};

#endif