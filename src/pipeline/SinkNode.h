#ifndef SINK_NODE_H_
#define SINK_NODE_H_

#include "core/ehm/EHM.h"

// Dream Pipeline
// dos/src/pipeline/SinkNode.h

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