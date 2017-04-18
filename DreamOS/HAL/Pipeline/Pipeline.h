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

	RESULT SetDestinationSinkNode(std::shared_ptr<SinkNode> pDestinationSinkNode);
	std::shared_ptr<SinkNode> GetDestinationSinkNode();

private:
	std::vector<std::shared_ptr<DNode>> m_nodes;

	std::shared_ptr<SinkNode> m_pDestinationSinkNode = nullptr;
};

#endif	// ! PIPELINE_H_