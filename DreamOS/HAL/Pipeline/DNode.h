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
	RESULT MakeConnection(std::string strName, CONNECTION_TYPE type, objType *pDestination, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE) {
		RESULT r = R_PASS;

		std::vector<DConnection*> *pDConnections = nullptr;

		// Ensure no connections exist with this name
		CBM((Connection(strName, type) == nullptr), "%s Connection %s already exists", ConnectionTypeString(type).c_str(), strName.c_str());

		pDConnections = GetConnectionSet(type);
		CN(pDConnections);

		// Create the connection
		{
			DConnection* pDConnection = DConnection::MakeDConnection<objType>(this, strName, type, pDestination, optFlags);
			CN(pDConnection);

			pDConnections->push_back(pDConnection);
		}

	Error:
		return r;
	}

	template <class objType>
	RESULT MakeConnection(std::string strName, CONNECTION_TYPE type, objType **ppDestination, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE) {
		RESULT r = R_PASS;

		std::vector<DConnection*> *pDConnections = nullptr;

		// Ensure no connections exist with this name
		CBM((Connection(strName, type) == nullptr), "%s Connection %s already exists", ConnectionTypeString(type).c_str(), strName.c_str());

		pDConnections = GetConnectionSet(type);
		CN(pDConnections);

		// Create the connection
		{
			DConnection* pDConnection = DConnection::MakeDConnection<objType>(this, strName, type, ppDestination, optFlags);
			CN(pDConnection);

			pDConnections->push_back(pDConnection);
		}

	Error:
		return r;
	}

	template <class objType>
	RESULT MakeInput(std::string strName, objType **ppDestination, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE) {
		return MakeConnection<objType>(strName, CONNECTION_TYPE::INPUT, ppDestination, optFlags);
	}

	template <class objType>
	RESULT MakeOutput(std::string strName, objType *pDestination, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE) {
		return MakeConnection<objType>(strName, CONNECTION_TYPE::OUTPUT, pDestination, optFlags);
	}

	template <class objType>
	RESULT MakeOutputPassthru(std::string strName, objType **ppDestination) {
		return MakeConnection<objType>(strName, CONNECTION_TYPE::OUTPUT, ppDestination, DCONNECTION_FLAGS::PASSTHRU);
	}

	template <class objType>
	RESULT SetInput(std::string strName, objType *pDestination) {
		RESULT r = R_PASS;

		DConnection* pDConnection = nullptr;

		// Find the connection
		CNM((pDConnection = Connection(strName, CONNECTION_TYPE::INPUT)), "In Connection %s not found", strName.c_str());

		DConnectionTyped<objType> *pDConnectionTyped = dynamic_cast<DConnectionTyped<objType>*>(pDConnection);
		CN(pDConnectionTyped);

		CR(pDConnectionTyped->SetConnection(pDestination));

	Error:
		return r;
	}

	template <class objType>
	RESULT SetOutputPassthru(std::string strName, DConnection* pOutputConnection) {
		RESULT r = R_PASS;

		DConnection* pDConnection = nullptr;

		// Find the connection
		CNM((pDConnection = Connection(strName, CONNECTION_TYPE::OUTPUT)), "Output Connection %s not found", strName.c_str());
		CBM((pDConnection->IsPassthru()), "Output Connection %s is not set to pass thru", strName.c_str())

		DConnectionTyped<objType> *pDConnectionTyped = dynamic_cast<DConnectionTyped<objType>*>(pDConnection);
		CN(pDConnectionTyped);

		DConnectionTyped<objType> *pDConnectionTypedOutput = dynamic_cast<DConnectionTyped<objType>*>(pOutputConnection);
		CN(pDConnectionTypedOutput);

		//CR(pDConnectionTyped->SetConnection(pDestination));
		CR(pDConnectionTyped->SetConnectionPassthru(pDConnectionTypedOutput));

	Error:
		return r;
	}

	size_t GetNumInputConnections();
	size_t GetNumOutputConnections();
	size_t GetNumConnections(CONNECTION_TYPE type);
	size_t GetNumConnections();

	DConnection* Connection(std::string strName, CONNECTION_TYPE type);
	DConnection* Input(std::string strName);
	DConnection* Output(std::string strName);

	std::string GetName();
	RESULT SetName(std::string strName);

	virtual RESULT SetupConnections() = 0;

	RESULT Connect(DConnection* pInputConnection, DConnection* pOutputConnection);
	RESULT ConnectToInput(std::string strInputName, DConnection* pOutputConnection);
	RESULT ConnectToOutput(std::string strOutputName, DConnection* pInputConnection);
	RESULT ConnectToAllInputs(DConnection* pOutputConnection);

	RESULT Disconnect();

	virtual RESULT RenderNode(long frameID = 0);
	virtual RESULT ProcessNode(long frameID = 0) { return R_NOT_IMPLEMENTED; }
	virtual RESULT PreProcessNode(long frameID = 0) { return R_NOT_IMPLEMENTED; }

	// Allows for the early termination of a path
	// This will prevent connections from rendering / processing
	RESULT Terminate();

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

	bool m_fTerminate = false;

	// TODO: Move to a pattern
public:
	int incRefCount() { m_refCount++; return m_refCount; }
	int decRefCount() { m_refCount--; return m_refCount; }
	int refCount() { return m_refCount; }

private:
	int m_refCount = 0;

private:
	std::string m_strName;
};

#endif	// ! DNODE_H_