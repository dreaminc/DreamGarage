#ifndef PIPELINE_H_
#define PIPELINE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/Pipeline.h
// The Pipeline is the container class for the RenderNodes that constitute the
// Dream rendering network.  

#include "Primitives/DObject.h"

#include <vector>
#include <memory>

class DNode;

class Pipeline : public DObject {
	Pipeline();
	~Pipeline();
	
	std::shared_ptr<DNode> FindNode(std::string strName);

private:
	std::vector<std::shared_ptr<DNode>> m_nodes;
	std::shared_ptr<DNode> m_pOutputNode = nullptr;
};

#endif	// ! PIPELINE_H_