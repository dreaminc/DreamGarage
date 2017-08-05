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
	RESULT r = R_PASS;

	CR(r);
	// TODO: 

Error:
	return;
}

RESULT DNode::ClearInputConnections() {
	m_inputs.clear();
	return R_PASS;
}

RESULT DNode::ClearOutputConnections() {
	m_outputs.clear();
	return R_PASS;
}

RESULT DNode::ClearConnections() {
	RESULT r = R_PASS;

	CR(ClearInputConnections());
	CR(ClearOutputConnections());

Error:
	return r;
}

/*
template <class objType>
RESULT DNode::MakeConnection(std::string strName, CONNECTION_TYPE type, objType*&pDestination) {
	RESULT r = R_PASS;

	std::vector<DConnection*> *pDConnections = nullptr;

	// Ensure no connections exist with this name
	CBM((Connection(strName, type) == nullptr), "%s Connection %s already exists", ConnectionTypeString(type).c_str(), strName.c_str());

	pDConnections = GetConnectionSet(type);
	CN(pDConnections);

	// Create the connection
	{
		DConnection* pDConnection = DConnection::MakeConnection<objType>(this, strName, type, pDestination);
		CN(pDConnection);

		pDConnections->push_back(pDConnection);
	}

Error:
	return r;
}

template <class objType>
RESULT DNode::MakeInput(std::string strName, objType*&pDestination) {
	return MakeConnection<objType>(strName, CONNECTION_TYPE::INPUT, pDestination);
}

template <class objType>
RESULT DNode::MakeOutput(std::string strName, objType*&pDestination) {
	return MakeConnection<objType>(strName, CONNECTION_TYPE::OUTPUT, pDestination);
}
*/

size_t DNode::GetNumConnections() {
	return (GetNumConnections(CONNECTION_TYPE::INPUT)) + (GetNumConnections(CONNECTION_TYPE::OUTPUT));
}

size_t DNode::GetNumInputConnections() {
	return GetNumConnections(CONNECTION_TYPE::INPUT);
}

size_t DNode::GetNumOutputConnections() {
	return GetNumConnections(CONNECTION_TYPE::OUTPUT);
}

size_t DNode::GetNumConnections(CONNECTION_TYPE type) {
	RESULT r = R_PASS;
	size_t retSize = 0;

	std::vector<DConnection*> *pDConnections = GetConnectionSet(type);
	CN(pDConnections);

	for (auto &pDConnection : *pDConnections) {
		retSize += pDConnection->GetNumConnections();
	}

Error:
	return retSize;
}

std::vector<DConnection*>* DNode::GetConnectionSet(CONNECTION_TYPE type) {
	if (type == CONNECTION_TYPE::INPUT) {
		return &m_inputs;
	}
	else if (type == CONNECTION_TYPE::OUTPUT) {
		return &m_outputs;
	}

	return nullptr;
}

DConnection* DNode::Connection(std::string strName, CONNECTION_TYPE type) {
	RESULT r = R_PASS;

	std::vector<DConnection*> *pDConnections = GetConnectionSet(type);
	CN(pDConnections);

	for (auto &pDConnection : *pDConnections) {
		if (pDConnection->GetName() == strName) {
			return pDConnection;
		}
	}

Error:
	return nullptr;
}

DConnection* DNode::Input(std::string strName) {
	return Connection(strName, CONNECTION_TYPE::INPUT);
}

DConnection* DNode::Output(std::string strName) {
	return Connection(strName, CONNECTION_TYPE::OUTPUT);
}


std::string DNode::GetName() {
	return m_strName;
}

RESULT DNode::SetName(std::string strName) {
	m_strName = strName;
	return R_PASS;
}

RESULT DNode::Connect(DConnection* pInputConnection, DConnection* pOutputConnection) {
	RESULT r = R_PASS;

	CN(pInputConnection);
	CN(pOutputConnection);
	CB(pInputConnection->GetType() == CONNECTION_TYPE::INPUT);
	CB(pOutputConnection->GetType() == CONNECTION_TYPE::OUTPUT);

	CRM(pInputConnection->Connect(pOutputConnection), "Failed to connect connections together");

Error:
	return r;
}

// This can be used with sink nodes where all inputs connect to the same network
RESULT DNode::ConnectToAllInputs(DConnection* pOutputConnection) {
	RESULT r = R_PASS;

	std::vector<DConnection*> *pDConnections = GetConnectionSet(CONNECTION_TYPE::INPUT);
	CN(pDConnections);

	for (auto &pDConnection : *pDConnections) {
		CR(Connect(pDConnection, pOutputConnection));
	}

Error:
	return r;
}

RESULT DNode::ConnectToInput(std::string strInputName, DConnection* pOutputConnection) {
	RESULT r = R_PASS;

	auto pInputConnection = Input(strInputName);

	CNM(pInputConnection, "%s input not found", strInputName.c_str());
	CN(pOutputConnection);
	CB(pOutputConnection->GetType() == CONNECTION_TYPE::OUTPUT);

	CR(Connect(pInputConnection, pOutputConnection));

Error:
	return r;
}

RESULT DNode::ConnectToOutput(std::string strOutputName, DConnection* pInputConnection) {
	RESULT r = R_PASS;

	auto pOutputConnection = Output(strOutputName);
	CNM(pOutputConnection, "%s output not found", strOutputName.c_str());
	CN(pInputConnection);
	CB(pInputConnection->GetType() == CONNECTION_TYPE::INPUT);

	CR(Connect(pInputConnection, pOutputConnection));

Error:
	return r;
}

RESULT DNode::Disconnect() {
	RESULT r = R_PASS;

	for (auto &pConnection : m_inputs) {
		CR(pConnection->Disconnect());
	}

	for (auto &pConnection : m_outputs) {
		CR(pConnection->Disconnect());
	}

Error:
	return r;
}

RESULT DNode::Terminate() {
	m_fTerminate = true;
	return R_PASS;
}

// RenderNode can be overridden but this should be an exceptionally rare thing 
// since a node that abuses RenderNode can basically kill a network
RESULT DNode::RenderNode(long frameID) {
	RESULT r = R_PASS;

	// This will reset the terminate for the node which may choose to set 
	// it later.

	// For recursive nodes, this will permeate since PreProcessNode will have 
	// already be called 
	m_fTerminate = false;

	// TODO: Handle frameID to prevent repeated node renders

	// This allows a node to process stuff before it's connections
	// This is used often in recursive type nodes
	// TODO: We might want to revisit this in the future
	CR(PreProcessNode(frameID));

	if (m_fTerminate == true) {
		return r;
	}

	// First Render input nodes
	for (auto &pInputConnection : m_inputs) {
		pInputConnection->RenderConnections(frameID);

		if (m_fTerminate == true) {
			return r;
		}
	}

	// Pass processing over to extended node
	CR(ProcessNode(frameID));

Error:
	return r;
}