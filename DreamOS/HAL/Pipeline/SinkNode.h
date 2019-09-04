#ifndef SINK_NODE_H_
#define SINK_NODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/SinkNode.h
// Sink nodes effectively sink sources which can either be program nodes
// or otherwise

#include "DNode.h"

class SinkNode : public DNode {
public:
	SinkNode(std::string strName);
	~SinkNode();

	//virtual RESULT SetupConnections() = 0;

private:
	// empty
};

#endif	// SINK_NODE_H_