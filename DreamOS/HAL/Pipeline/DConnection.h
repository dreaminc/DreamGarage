#ifndef D_CONNECTION_H_
#define D_CONNECTION_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/Pipeline/DNode.h
// The Dream Node object

#include "Primitives/DObject.h"

#include <vector>

#include "PipelineCommon.h"

class DConnection;
template <class objType> class DConnectionTyped;

class DConnection : public DObject {
	template <class objType> friend class DConnectionTyped;

protected:
	DConnection(DNode* pParentNode, CONNECTION_TYPE connType, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE);
	DConnection(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE);

public:
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
	bool IsActive();

	RESULT RenderConnections(long frameID = 0);
	RESULT RenderParent(long frameID = 0);

	virtual RESULT LinkInputToOutputObjects(DConnection* pInputConnection, DConnection* pOutputConnection) = 0;

	template <class objType>
	static DConnection* MakeDConnection(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType *pObject, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE) {
		
		///*
		RESULT r = R_PASS;

		DConnectionTyped<objType> *pConnectionTyped = nullptr;

		pConnectionTyped = new DConnectionTyped<objType>(pParentNode, strName, connType, pObject, optFlags);
		CN(pConnectionTyped);

		// Success:
		return (DConnection*)(pConnectionTyped);

	Error:
		//*/
		return nullptr;
	}

	template <class objType>
	static DConnection* MakeDConnection(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType **ppObject, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE) {

		///*
		RESULT r = R_PASS;

		DConnectionTyped<objType> *pConnectionTyped = nullptr;

		pConnectionTyped = new DConnectionTyped<objType>(pParentNode, strName, connType, ppObject, optFlags);
		CN(pConnectionTyped);

		// Success:
		return (DConnection*)(pConnectionTyped);

	Error:
		//*/
		return nullptr;
	}

private:
	DCONNECTION_FLAGS m_flags;
	DNode* m_pParentNode = nullptr;
	std::vector<DConnection*> m_connections;

private:
	CONNECTION_TYPE m_connType;
	std::string m_strName;
};

template <class objType>
class DConnectionTyped : public DConnection {
public:

	DConnectionTyped(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType *pObject, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE) :
		DConnection(pParentNode, strName, connType, optFlags),
		m_pObject(pObject),
		m_ppObject(nullptr)
	{
		// empty
	}

	DConnectionTyped(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType **ppObject, DCONNECTION_FLAGS optFlags = DCONNECTION_FLAGS::NONE) :
		DConnection(pParentNode, strName, connType, optFlags),
		m_pObject(nullptr),
		m_ppObject(ppObject)
	{
		// empty
	}

	virtual RESULT LinkInputToOutputObjects(DConnection* pInputConnection, DConnection* pOutputConnection) override {
		RESULT r = R_PASS;

		CB(pInputConnection->GetType() == CONNECTION_TYPE::INPUT);
		CB(pOutputConnection->GetType() == CONNECTION_TYPE::OUTPUT);

		DConnectionTyped<objType>* pTypedInputConnection = dynamic_cast<DConnectionTyped<objType>*>(pInputConnection);
		CN(pTypedInputConnection);

		DConnectionTyped<objType>* pTypedOutputConnection = dynamic_cast<DConnectionTyped<objType>*>(pOutputConnection);
		CN(pTypedOutputConnection);

		CN(pTypedInputConnection->m_ppObject);
		CN(pTypedOutputConnection->m_pObject);

		// Set the destination?
		*pTypedInputConnection->m_ppObject = pTypedOutputConnection->m_pObject;

	Error:
		return r;
	}

private:
	objType *m_pObject = nullptr;
	objType **m_ppObject = nullptr;
};


#endif	// ! D_CONNECTION_H_