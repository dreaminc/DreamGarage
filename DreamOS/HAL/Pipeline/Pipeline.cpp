#include "Pipeline.h"

#include "DNode.h"
#include "SinkNode.h"

Pipeline::Pipeline() {
	// empty
}

Pipeline::~Pipeline() {
	// empty
}

std::shared_ptr<DNode> Pipeline::FindNode(std::string strName) {
	for (auto &pNode : m_nodes) {
		if (pNode->GetName() == strName) {
			return pNode;
		}
	}

	return nullptr;
}

RESULT Pipeline::RunPipeline() {
	RESULT r = R_PASS;

	CR(m_pDestinationSinkNode->RenderNode(m_frameID++));

Error:
	return r;
}

RESULT Pipeline::SetDestinationSinkNode(SinkNode* pDestinationSinkNode) {
	RESULT r = R_PASS;
	
	CN(pDestinationSinkNode);
	CBM((m_pDestinationSinkNode == nullptr), "Destination sink node already set - please disconnect it first");
	
	m_pDestinationSinkNode = pDestinationSinkNode;

Error:
	return r;
}

SinkNode* Pipeline::GetDestinationSinkNode() {
	return m_pDestinationSinkNode;
}