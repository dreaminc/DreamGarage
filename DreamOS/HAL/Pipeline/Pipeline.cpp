#include "Pipeline.h"

#include "DNode.h"

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