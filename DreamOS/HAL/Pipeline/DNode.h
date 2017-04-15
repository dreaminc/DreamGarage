#ifndef DNODE_H_
#define DNODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/DNode.h
// The Dream Node object

#include "Primitives/DObject.h"

#include <vector>
#include <memory>

class DConnection;
enum class CONNECTION_TYPE;

class DNode : public DObject {

public:
	DNode();
	DNode(std::string strName);
	~DNode();

	RESULT ClearInputs();
	RESULT ClearOutputs();
	RESULT ClearInOut();

	RESULT MakeConnection(std::string strName, CONNECTION_TYPE type);
	RESULT MakeInput(std::string strName);
	RESULT MakeOutput(std::string strName);

	std::shared_ptr<DConnection> FindConnection(std::string strName, CONNECTION_TYPE type);
	std::shared_ptr<DConnection> FindInput(std::string strName);
	std::shared_ptr<DConnection> FindOutput(std::string strName);

	std::string GetName();
	RESULT SetName(std::string strName);

private:
	std::vector<std::shared_ptr<DConnection>>* GetConnectionSet(CONNECTION_TYPE type);

private:
	std::vector<std::shared_ptr<DConnection>> m_inputs;
	std::vector<std::shared_ptr<DConnection>> m_outputs;

private:
	std::string m_strName;
};

#endif	// ! DNODE_H_