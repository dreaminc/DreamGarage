#ifndef HMD_SINK_NODE_H_
#define HMD_SINK_NODE_H_

#include "core/ehm/EHM.h"

// Dream HMD
// dos/src/dos/HMDSinkNode.h

// The HMD Sink Node class 
// This class should be subclassed by the various HMDs as a display
// to unify the arch

#include "pipeline/SinkNode.h"

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