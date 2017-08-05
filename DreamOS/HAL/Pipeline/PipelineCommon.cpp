#include "PipelineCommon.h"

// Utility
std::string ConnectionTypeString(CONNECTION_TYPE type) {
	switch (type) {
	case CONNECTION_TYPE::INPUT: return "input"; break;
	case CONNECTION_TYPE::OUTPUT: return "output"; break;

	default:
	case CONNECTION_TYPE::INVALID: return "invalid"; break;
	}

	return "invalid";
}