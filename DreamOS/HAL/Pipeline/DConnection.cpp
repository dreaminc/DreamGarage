#include "DConnection.h"

#include "DNode.h"

std::string ConnectionTypeString(CONNECTION_TYPE type) {
	switch (type) {
		case CONNECTION_TYPE::INPUT: return "input"; break;
		case CONNECTION_TYPE::OUTPUT: return "output"; break;
		
		default:
		case CONNECTION_TYPE::INVALID: return "invalid"; break;
	}

	return "invalid";
}

DConnection::DConnection(std::shared_ptr<DNode> pParentNode, CONNECTION_TYPE connType) :
	m_pParentNode(pParentNode),
	m_connType(connType)
{
	// empty
}

DConnection::DConnection(std::shared_ptr<DNode> pParentNode, std::string strName, CONNECTION_TYPE connType) :
	m_pParentNode(pParentNode),
	m_strName(strName),
	m_connType(connType)
{
	// empty
}

DConnection::~DConnection() {
	// empty
}

std::string DConnection::GetName() {
	return m_strName;
}

std::string DConnection::GetParentName() {
	if (m_pParentNode == nullptr)
		return "invalid";
	else
		return m_pParentNode->GetName();
}

RESULT DConnection::SetName(std::string strName) {
	m_strName = strName;
	return R_PASS;
}

std::shared_ptr<DConnection> DConnection::FindConnection(std::shared_ptr<DConnection> pConnection) {
	for (auto &pConnectedConnection : m_connections) {
		if (pConnectedConnection == pConnection) {
			return pConnectedConnection;
		}
	}

	return nullptr;
}

std::shared_ptr<DConnection> DConnection::FindConnection(std::string strConnectionName, std::string strNodeName) {
	for (auto &pConnection : m_connections) {
		if (pConnection->GetName() == strConnectionName && pConnection->GetParentName() == strNodeName) {
			return pConnection;
		}
	}

	return nullptr;
}

RESULT DConnection::Connect(std::shared_ptr<DConnection> pConnection) {
	RESULT r = R_PASS;

	std::shared_ptr<DConnection> pThisConnection = std::shared_ptr<DConnection>(this);

	// First check this is not already connected
	CBM((FindConnection(pConnection) == nullptr), "Connection is already connected to this connection");
	CBM((pConnection->FindConnection(pThisConnection) == nullptr), "This connection is already connected to connection");

	// Mutually connect the connections here

	DEBUG_LINEOUT("Connecting %s:%s.%s to %s:%s.%s",
		ConnectionTypeString(m_connType).c_str(), m_pParentNode->GetName().c_str(), GetName().c_str(),
		ConnectionTypeString(pConnection->m_connType).c_str(), pConnection->m_pParentNode->GetName().c_str(), pConnection->GetName().c_str());

	pConnection->m_connections.push_back(pThisConnection);
	m_connections.push_back(pConnection);

Error:
	return r;
}

CONNECTION_TYPE DConnection::GetType() {
	return m_connType;
}
