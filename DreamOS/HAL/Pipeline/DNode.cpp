#include "DNode.h"

#include "DConnection.h"

DNode::DNode() {
	// empty
}

DNode::DNode(std::string strName) :
	m_strName(strName)
{
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
	CBM((Connection(strName, type) == nullptr), "%s Connection %s already exists", ConnectionTypeString(type).c_str(), strName.c_str());

	pDConnections = GetConnectionSet(type);
	CN(pDConnections);

	// Create the connection
	{
		std::shared_ptr<DConnection> pDConnection = std::make_shared<DConnection>(std::shared_ptr<DNode>(this), strName, type);
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

std::shared_ptr<DConnection> DNode::Connection(std::string strName, CONNECTION_TYPE type) {
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

std::shared_ptr<DConnection> DNode::Input(std::string strName) {
	return Connection(strName, CONNECTION_TYPE::INPUT);
}

std::shared_ptr<DConnection> DNode::Output(std::string strName) {
	return Connection(strName, CONNECTION_TYPE::OUTPUT);
}


std::string DNode::GetName() {
	return m_strName;
}

RESULT DNode::SetName(std::string strName) {
	m_strName = strName;
	return R_PASS;
}

RESULT DNode::Connect(std::shared_ptr<DConnection> pInputConnection, std::shared_ptr<DConnection> pOutputConnection) {
	RESULT r = R_PASS;

	CN(pInputConnection);
	CN(pOutputConnection);
	CB(pInputConnection->GetType() == CONNECTION_TYPE::INPUT);
	CB(pOutputConnection->GetType() == CONNECTION_TYPE::OUTPUT);

	CRM(pInputConnection->Connect(pOutputConnection), "Failed to connect connections together");

Error:
	return r;
}

RESULT DNode::ConnectToInput(std::string strInputName, std::shared_ptr<DConnection> pOutputConnection) {
	RESULT r = R_PASS;

	auto pInputConnection = Input(strInputName);

	CNM(pInputConnection, "%s input not found", strInputName.c_str());
	CN(pOutputConnection);
	CB(pOutputConnection->GetType() == CONNECTION_TYPE::OUTPUT);

	CR(Connect(pInputConnection, pOutputConnection));

Error:
	return r;
}

RESULT DNode::ConnectToOutput(std::string strOutputName, std::shared_ptr<DConnection> pInputConnection) {
	RESULT r = R_PASS;

	auto pOutputConnection = Output(strOutputName);
	CNM(pOutputConnection, "%s output not found", strOutputName.c_str());
	CN(pInputConnection);
	CB(pInputConnection->GetType() == CONNECTION_TYPE::INPUT);

	CR(Connect(pInputConnection, pOutputConnection));

Error:
	return r;
}