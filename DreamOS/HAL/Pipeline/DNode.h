#ifndef DNODE_H_
#define DNODE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/DNode.h
// The Dream Node object

#include "Primitives/DObject.h"

#include <vector>

#include "PipelineCommon.h"
#include "DConnection.h"

class DNode : public DObject {

public:
	DNode();
	DNode(std::string strName);
	~DNode();

	RESULT ClearInputConnections();
	RESULT ClearOutputConnections();
	RESULT ClearConnections();

	template <class objType>
	RESULT MakeConnection(std::string strName, CONNECTION_TYPE type, objType*&pDestination) {
		RESULT r = R_PASS;

		std::vector<DConnection*> *pDConnections = nullptr;

		// Ensure no connections exist with this name
		CBM((Connection(strName, type) == nullptr), "%s Connection %s already exists", ConnectionTypeString(type).c_str(), strName.c_str());

		pDConnections = GetConnectionSet(type);
		CN(pDConnections);

		// Create the connection
		{
			DConnection* pDConnection = DConnection::MakeDConnection<objType>(this, strName, type, pDestination);
			CN(pDConnection);

			pDConnections->push_back(pDConnection);
		}

	Error:
		return r;
	}

	template <class objType>
	RESULT MakeInput(std::string strName, objType*&pDestination) {
		return MakeConnection<objType>(strName, CONNECTION_TYPE::INPUT, pDestination);
	}

	template <class objType>
	RESULT MakeOutput(std::string strName, objType*&pDestination) {
		return MakeConnection<objType>(strName, CONNECTION_TYPE::OUTPUT, pDestination);
	}

	DConnection* Connection(std::string strName, CONNECTION_TYPE type);
	DConnection* Input(std::string strName);
	DConnection* Output(std::string strName);

	std::string GetName();
	RESULT SetName(std::string strName);

	virtual RESULT SetupConnections() = 0;

	RESULT Connect(DConnection* pInputConnection, DConnection* pOutputConnection);
	RESULT ConnectToInput(std::string strInputName, DConnection* pOutputConnection);
	RESULT ConnectToOutput(std::string strOutputName, DConnection* pInputConnection);

	RESULT RenderNode(long frameID = 0);
	virtual RESULT ProcessNode(long frameID = 0) = 0;

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