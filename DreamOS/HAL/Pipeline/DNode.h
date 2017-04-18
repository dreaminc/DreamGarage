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

	std::shared_ptr<DConnection> Connection(std::string strName, CONNECTION_TYPE type);
	std::shared_ptr<DConnection> Input(std::string strName);
	std::shared_ptr<DConnection> Output(std::string strName);

	std::string GetName();
	RESULT SetName(std::string strName);

	virtual RESULT SetupConnections() = 0;

	RESULT Connect(std::shared_ptr<DConnection> pInputConnection, std::shared_ptr<DConnection> pOutputConnection);
	RESULT ConnectToInput(std::string strInputName, std::shared_ptr<DConnection> pOutputConnection);
	RESULT ConnectToOutput(std::string strOutputName, std::shared_ptr<DConnection> pInputConnection);

	virtual RESULT ProcessNode() = 0;

	template <class nodeType, class... nodeArgsTypes>
	static std::shared_ptr<nodeType> MakeNode(nodeArgsTypes&&... sinkArgs) {
		RESULT r = R_PASS;

		std::shared_ptr<nodeType> pNode = std::make_shared<nodeType>(sinkArgs...);
		CN(pNode);

		CR(pNode->SetupConnections());

		//Success:
		return pNode;

	Error:
		pNode = nullptr;
		return nullptr;
	}

private:
	std::vector<std::shared_ptr<DConnection>>* GetConnectionSet(CONNECTION_TYPE type);

private:
	std::vector<std::shared_ptr<DConnection>> m_inputs;
	std::vector<std::shared_ptr<DConnection>> m_outputs;

private:
	std::string m_strName;
};

#endif	// ! DNODE_H_