#ifndef PIPELINE_COMMON_H_
#define PIPELINE_COMMON_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/PipelineCommon.h

#include <string>

enum class CONNECTION_TYPE {
	INPUT,
	OUTPUT,
	INVALID
};

class DNode;
class DConnection;

std::string ConnectionTypeString(CONNECTION_TYPE type);

#endif	// ! PIPELINE_COMMON_H_