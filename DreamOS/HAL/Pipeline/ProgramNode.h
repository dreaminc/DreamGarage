#ifndef PROGRAM_NODE_H_
#define PROGRAM_NODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/ProgramNode.h
// A program node is a DNode that takes in a variety of sources and outputs
// to a sink, usually along with some form of "program" that produces new output
// data of some kind

#include "DNode.h"

class ProgramNode : public DNode {
public:
	ProgramNode(std::string strName, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);
	virtual ~ProgramNode();

	virtual RESULT SetupConnections() = 0;
	virtual RESULT ProcessNode(long frameID = 0) = 0;
};

#endif	// PROGRAM_NODE_H_