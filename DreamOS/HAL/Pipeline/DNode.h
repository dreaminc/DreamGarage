#ifndef DNODE_H_
#define DNODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/DNode.h
// The Dream Node object

#include "Primitives/DObject.h"

#include <vector>

class DConnection;
enum class CONNECTION_TYPE;

class DNode : public DObject {

public:
	DNode();
	DNode(std::string strName);
	~DNode();

	RESULT ClearInputConnections();
	RESULT ClearOutputConnections();
	RESULT ClearConnections();

	RESULT MakeConnection(std::string strName, CONNECTION_TYPE type);
	RESULT MakeInput(std::string strName);
	RESULT MakeOutput(std::string strName);

	DConnection* Connection(std::string strName, CONNECTION_TYPE type);
	DConnection* Input(std::string strName);
	DConnection* Output(std::string strName);

	std::string GetName();
	RESULT SetName(std::string strName);

	virtual RESULT SetupConnections() = 0;

	RESULT Connect(DConnection* pInputConnection, DConnection* pOutputConnection);
	RESULT ConnectToInput(std::string strInputName, DConnection* pOutputConnection);
	RESULT ConnectToOutput(std::string strOutputName, DConnection* pInputConnection);

	virtual RESULT ProcessNode() = 0;

	template <class nodeType, class... nodeArgsTypes>
	static nodeType* MakeNode(nodeArgsTypes&&... sinkArgs) {
		RESULT r = R_PASS;

		nodeType *pNode = new nodeType(sinkArgs...);
		CN(pNode);

		CR(pNode->SetupConnections());

		//Success:
		return pNode;

	Error:
		pNode = nullptr;
		return nullptr;
	}

private:
	std::vector<DConnection*>* GetConnectionSet(CONNECTION_TYPE type);

private:
	std::vector<DConnection*> m_inputs;
	std::vector<DConnection*> m_outputs;

private:
	std::string m_strName;
};

#endif	// ! DNODE_H_