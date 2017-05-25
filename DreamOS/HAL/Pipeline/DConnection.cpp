#include "DConnection.h"

#include "DNode.h"
#include "ProgramNode.h"

DConnection::DConnection(DNode* pParentNode, CONNECTION_TYPE connType, DCONNECTION_FLAGS optFlags) :
	m_pParentNode(pParentNode),
	m_connType(connType),
	m_flags(optFlags)
{
	// empty
}

DConnection::DConnection(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, DCONNECTION_FLAGS optFlags) :
	m_pParentNode(pParentNode),
	m_strName(strName),
	m_connType(connType),
	m_flags(optFlags)
{
	// empty
}

DConnection::~DConnection() {
	RESULT r = R_PASS;

	CR(Disconnect());

Error:
	return;
}

RESULT DConnection::RemoveConnection(DConnection* pConnection) {
	for (auto it = m_connections.begin(); it < m_connections.end(); it++) {
		if ((*it) == pConnection) {
			m_connections.erase(it);
			return R_PASS;
		}
	}

	return R_NOT_FOUND;
}

RESULT DConnection::Disconnect(DConnection* pRemoteConnection) {
	RESULT r = R_PASS;

	CNM(FindConnection(pRemoteConnection), "Failed to find that connection");
	CR(pRemoteConnection->RemoveConnection(this));
	CR(RemoveConnection(pRemoteConnection));

	DEBUG_LINEOUT("Disconnecting %s:%s.%s to %s:%s.%s",
		ConnectionTypeString(m_connType).c_str(), m_pParentNode->GetName().c_str(), GetName().c_str(),
		ConnectionTypeString(pRemoteConnection->m_connType).c_str(), pRemoteConnection->m_pParentNode->GetName().c_str(), pRemoteConnection->GetName().c_str());

	if (pRemoteConnection->m_pParentNode != nullptr) {
		CR(pRemoteConnection->m_pParentNode->Disconnect());
	}

Error:
	return r;
}

RESULT DConnection::Disconnect() {
	RESULT r = R_PASS;

	// Remove from all remote connections
	while (m_connections.size() > 0) {
		auto pRemoteConnection = m_connections.back();

		CR(Disconnect(pRemoteConnection));

		DNode *pNode = pRemoteConnection->m_pParentNode;
		if (pNode != nullptr) {
			if (pNode->GetNumConnections() == 0 && pNode->refCount() == 0) {
				DEBUG_LINEOUT("Node %s has zero connections - deleting", pNode->GetName().c_str());

				delete pNode;
				pNode = nullptr;
			}
		}
	}

Error:
	return r;
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

size_t DConnection::GetNumConnections() {
	return m_connections.size();
}

DConnection* DConnection::FindConnection(DConnection* pConnection) {
	for (auto &pConnectedConnection : m_connections) {
		if (pConnectedConnection == pConnection) {
			return pConnectedConnection;
		}
	}

	return nullptr;
}

DConnection* DConnection::FindConnection(std::string strConnectionName, std::string strNodeName) {
	for (auto &pConnection : m_connections) {
		if (pConnection->GetName() == strConnectionName && pConnection->GetParentName() == strNodeName) {
			return pConnection;
		}
	}

	return nullptr;
}

RESULT DConnection::Connect(DConnection* pConnection) {
	RESULT r = R_PASS;

	// First check this is not already connected
	CBM((FindConnection(pConnection) == nullptr), "Connection is already connected to this connection");
	CBM((pConnection->FindConnection(this) == nullptr), "This connection is already connected to connection");

	// Mutually connect the connections here

	DEBUG_LINEOUT("Connecting %s:%s.%s to %s:%s.%s",
		ConnectionTypeString(m_connType).c_str(), m_pParentNode->GetName().c_str(), GetName().c_str(),
		ConnectionTypeString(pConnection->m_connType).c_str(), pConnection->m_pParentNode->GetName().c_str(), pConnection->GetName().c_str());

	pConnection->m_connections.push_back(this);
	m_connections.push_back(pConnection);

	if(m_connType == CONNECTION_TYPE::INPUT)
		LinkInputToOutputObjects(this, pConnection);
	else
		LinkInputToOutputObjects(pConnection, this);

Error:
	return r;
}

CONNECTION_TYPE DConnection::GetType() {
	return m_connType;
}

bool DConnection::IsActive() {
	if (static_cast<int>(m_flags & DCONNECTION_FLAGS::PASSIVE) != 0)
		return false;
	else
		return true;
}

bool DConnection::IsPassthru() {
	if (static_cast<int>(m_flags & DCONNECTION_FLAGS::PASSTHRU) != 0)
		return true;
	else
		return false;
}

RESULT DConnection::RenderConnections(long frameID) {
	RESULT r = R_PASS;

	for (auto &pConnection : m_connections) {
		if (pConnection->IsActive()) {
			r = pConnection->RenderParent(frameID);

			if (r < 0)
				return r;
		}
	}

	return R_PASS;
}

RESULT DConnection::RenderParent(long frameID) {
	return m_pParentNode->RenderNode(frameID);
}