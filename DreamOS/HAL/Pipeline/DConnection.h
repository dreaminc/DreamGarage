#ifndef D_CONNECTION_H_
#define D_CONNECTION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/DNode.h
// The Dream Node object

#include "Primitives/DObject.h"

#include <vector>
#include <memory>

class DNode;

enum class CONNECTION_TYPE {
	INPUT,
	OUTPUT,
	INVALID
};

std::string ConnectionTypeString(CONNECTION_TYPE type);

class DConnection : public DObject {

public:
	DConnection(std::shared_ptr<DNode> pParentNode, CONNECTION_TYPE connType);
	DConnection(std::shared_ptr<DNode> pParentNode, std::string strName, CONNECTION_TYPE connType);

	~DConnection();

	std::string GetName();
	std::string GetParentName();
	RESULT SetName(std::string strName);

	std::shared_ptr<DConnection> FindConnection(std::shared_ptr<DConnection> pConnection);
	std::shared_ptr<DConnection> FindConnection(std::string strConnectionName, std::string strNodeName);

	RESULT Connect(std::shared_ptr<DConnection> pConnection);

	CONNECTION_TYPE GetType();

private:
	std::shared_ptr<DNode> m_pParentNode;
	std::vector<std::shared_ptr<DConnection>> m_connections;

private:
	CONNECTION_TYPE m_connType;
	std::string m_strName;
};

#endif	// ! D_CONNECTION_H_