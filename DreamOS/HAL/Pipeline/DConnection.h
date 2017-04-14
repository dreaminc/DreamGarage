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
	DConnection();
	DConnection(std::string strName);

	~DConnection();

	std::string GetName();
	RESULT SetName(std::string strName);

private:
	std::shared_ptr<DNode> m_pParentNode;
	std::vector<std::shared_ptr<DConnection>> m_connections;

private:
	std::string m_strName;
};

#endif	// ! D_CONNECTION_H_