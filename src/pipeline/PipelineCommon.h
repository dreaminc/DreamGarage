#ifndef PIPELINE_COMMON_H_
#define PIPELINE_COMMON_H_

#include "core/ehm/EHM.h"

// Dream Pipeline
// dos/src/pipeline/PipelineCommon.h

#include <string>

enum class CONNECTION_TYPE {
	INPUT,
	OUTPUT,
	INVALID
};

enum class PIPELINE_FLAGS : int {
	NONE			= 0,
	PASSIVE			= 1 << 0,
	PASSTHRU		= 1 << 1,
	ONESHOT			= 1 << 2,
	DIRTY			= 1 << 3
};

// taken from: http://stackoverflow.com/questions/12059774/c11-standard-conformant-bitmasks-using-enum-class
inline constexpr PIPELINE_FLAGS
operator&(PIPELINE_FLAGS __x, PIPELINE_FLAGS __y) {
	return static_cast<PIPELINE_FLAGS>
		(static_cast<int>(__x) & static_cast<int>(__y));
}

inline constexpr PIPELINE_FLAGS
operator|(PIPELINE_FLAGS __x, PIPELINE_FLAGS __y) {
	return static_cast<PIPELINE_FLAGS>
		(static_cast<int>(__x) | static_cast<int>(__y));
}

inline constexpr PIPELINE_FLAGS
operator~(PIPELINE_FLAGS __x) {
	return static_cast<PIPELINE_FLAGS>
		(~(static_cast<int>(__x)));
}

class DNode;
class DConnection;

std::string ConnectionTypeString(CONNECTION_TYPE type);

#endif	// ! PIPELINE_COMMON_H_