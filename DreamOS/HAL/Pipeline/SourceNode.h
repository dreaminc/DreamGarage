#ifndef SOURCE_NODE_H_
#define SOURCE_NODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/SourceNode.h
// Source nodes provide inputs to other nodes such as 
// program nodes or sink nodes

#include "DNode.h"

class SourceNode : public DNode {
public:
	SourceNode(std::string strName);
	~SourceNode();

private:
	// empty
};

#endif	// !SOURCE_NODE_H_