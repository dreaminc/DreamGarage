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
	RESULT RunAuxiliaryPipeline();
	RESULT Reset(bool fResetDestination = false);

	RESULT SetDestinationSinkNode(SinkNode* pDestinationSinkNode);
	SinkNode* GetDestinationSinkNode();

	RESULT SetAuxiliarySinkNode(SinkNode* pAuxiliarySinkNode);
	SinkNode* GetAuxiliarySinkNode();

private:
	std::vector<std::shared_ptr<DNode>> m_nodes;
	SinkNode* m_pDestinationSinkNode = nullptr;
	SinkNode* m_pAuxiliarySinkNode = nullptr;
	long m_frameID = 0;
};

#endif	// ! PIPELINE_H_