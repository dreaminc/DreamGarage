#ifndef HMD_SOURCE_NODE_H_
#define HMD_SOURCE_NODE_H_

#include "core/ehm/EHM.h"

// Dream HMD
// dos/src/hmd/HMDSourceNode.h

// The HMD Source Node class 
// This class should be sub classed by the various HMDs as a sense source node

#include "pipeline/SourceNode.h"

class HMDSourceNode : public SourceNode {
public:
	HMDSourceNode(std::string strSourceNodeName) :
		SourceNode(strSourceNodeName)
	{
		// empty
	}

	virtual RESULT SetupConnections() = 0;
	virtual RESULT ProcessNode(long frameID = 0) = 0;
};

#endif	// ! HMD_SOURCE_NODE_H_