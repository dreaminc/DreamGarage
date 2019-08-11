#ifndef SOURCE_NODE_H_
#define SOURCE_NODE_H_

#include "core/ehm/EHM.h"

// Dream Pipeline
// dos/src/pipeline/SourceNode.h

// Source nodes provide inputs to other nodes such as 
// program nodes or sink nodes

#include "DNode.h"

class SourceNode : public DNode {
public:
	SourceNode(std::string strName);
	~SourceNode();

	//virtual RESULT SetupConnections() = 0;
	//virtual RESULT ProcessNode(long frameID = 0) = 0;

private:
	// empty
};

#endif	// !SOURCE_NODE_H_