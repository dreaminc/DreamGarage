#ifndef HMD_SINK_NODE_H_
#define HMD_SINK_NODE_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/HMDSinkNode.h
// The HMD Sink Node class 
// This class should be subclassed by the various HMDs as a display
// to unify the arch


#include "HAL/Pipeline/SinkNode.h"

class HMDSinkNode : public SinkNode {
public:
	HMDSinkNode(std::string strSinkNodeName) : 
		SinkNode(strSinkNodeName)
	{
		// empty
	}

	virtual RESULT SetupConnections() = 0;
};


#endif	// ! HMD_SINK_NODE_H_