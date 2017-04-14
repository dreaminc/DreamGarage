#include "DNode.h"

#include "DConnection.h"

DNode::DNode() {
	// empty
}

DNode::~DNode() {
	// empty
}

RESULT DNode::ClearInputs() {
	m_inputs.clear();
	return R_PASS;
}

RESULT DNode::ClearOutputs() {
	m_outputs.clear();
	return R_PASS;
}

RESULT DNode::ClearInOut() {
	RESULT r = R_PASS;

	CR(ClearInputs());
	CR(ClearOutputs());

Error:
	return r;
}

std::vector<std::shared_ptr<DConnection>>* DNode::GetConnectionSet(CONNECTION_TYPE type) {
	if (type == CONNECTION_TYPE::INPUT) {
		return &m_inputs;
	}
	else if (type == CONNECTION_TYPE::OUTPUT) {
		return &m_outputs;
	}

	return nullptr;
}

RESULT DNode::MakeConnection(std::string strName, CONNECTION_TYPE type) {
	RESULT r = R_PASS;

	std::vector<std::shared_ptr<DConnection>> *pDConnections = nullptr;

	// Ensure no connections exist with this name
	CBM((FindConnection(strName, type) == nullptr), "%s Connection %s already exists", ConnectionTypeString(type).c_str(), strName.c_str());

	pDConnections = GetConnectionSet(type);
	CN(pDConnections);

	// Create the connection
	{
		std::shared_ptr<DConnection> pDConnection = std::make_shared<DConnection>(strName);
		CN(pDConnection);

		pDConnections->push_back(pDConnection);
	}

Error:
	return r;
}

RESULT DNode::MakeInput(std::string strName) {
	return MakeConnection(strName, CONNECTION_TYPE::INPUT);
}

RESULT DNode::MakeOutput(std::string strName) {
	return MakeConnection(strName, CONNECTION_TYPE::OUTPUT);
}

std::shared_ptr<DConnection> DNode::FindConnection(std::string strName, CONNECTION_TYPE type) {
	RESULT r = R_PASS;

	std::vector<std::shared_ptr<DConnection>> *pDConnections = GetConnectionSet(type);
	CN(pDConnections);

	for (auto &pDConnection : *pDConnections) {
		if (pDConnection->GetName() == strName) {
			return pDConnection;
		}
	}

Error:
	return nullptr;
}

std::shared_ptr<DConnection> DNode::FindInput(std::string strName) {
	return FindConnection(strName, CONNECTION_TYPE::INPUT);
}

std::shared_ptr<DConnection> DNode::FindOutput(std::string strName) {
	return FindConnection(strName, CONNECTION_TYPE::OUTPUT);
}


std::string DNode::GetName() {
	return m_strName;
}

RESULT DNode::SetName(std::string strName) {
	m_strName = strName;
	return R_PASS;
}