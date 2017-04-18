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
class SinkNode;

class Pipeline : public DObject {
public:
	Pipeline();
	~Pipeline();
	
	std::shared_ptr<DNode> FindNode(std::string strName);

	RESULT RunPipeline();

private:
	std::vector<std::shared_ptr<DNode>> m_nodes;

	std::shared_ptr<SinkNode> m_pOutputNode = nullptr;
};

#endif	// ! PIPELINE_H_