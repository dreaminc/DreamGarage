#ifndef D_CONNECTION_H_
#define D_CONNECTION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/DNode.h
// The Dream Node object

#include "Primitives/DObject.h"

#include <vector>

class DNode;

enum class CONNECTION_TYPE {
	INPUT,
	OUTPUT,
	INVALID
};

std::string ConnectionTypeString(CONNECTION_TYPE type);

class DConnection : public DObject {

public:
	DConnection(DNode* pParentNode, CONNECTION_TYPE connType);
	DConnection(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType);

	~DConnection();

	std::string GetName();
	std::string GetParentName();
	RESULT SetName(std::string strName);

	DConnection* FindConnection(DConnection* pConnection);
	DConnection* FindConnection(std::string strConnectionName, std::string strNodeName);

	RESULT RemoveConnection(DConnection* pConnection);

	RESULT Connect(DConnection* pConnection);
	RESULT Disconnect(DConnection* pConnection);
	RESULT Disconnect();

	CONNECTION_TYPE GetType();

private:
	DNode* m_pParentNode = nullptr;
	std::vector<DConnection*> m_connections;

private:
	CONNECTION_TYPE m_connType;
	std::string m_strName;
};

#endif	// ! D_CONNECTION_H_