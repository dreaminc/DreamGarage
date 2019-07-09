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

	friend class DNode;

protected:
	DConnection(DNode* pParentNode, CONNECTION_TYPE connType, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);
	DConnection(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

public:
	~DConnection();

	std::string GetName();
	std::string GetParentName();
	RESULT SetName(std::string strName);

	size_t GetNumConnections();
	DConnection* FindConnection(DConnection* pConnection);
	DConnection* FindConnection(std::string strConnectionName, std::string strNodeName);

	RESULT RemoveConnection(DConnection* pConnection);

	RESULT Connect(DConnection* pConnection);
	
	RESULT Disconnect();
	RESULT Disconnect(DConnection* pConnection);

	CONNECTION_TYPE GetType();
	bool IsActive();
	bool IsPassthru();

	RESULT RenderConnections(long frameID = 0);
	RESULT RenderParent(long frameID = 0);
	RESULT SetConnectionsDirty(long frameID = 0);

	virtual RESULT LinkInputToOutputObjects(DConnection* pInputConnection, DConnection* pOutputConnection) = 0;

	template <class objType>
	static DConnection* MakeDConnection(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType *pObject, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE) {
		
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
	static DConnection* MakeDConnection(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType **ppObject, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE) {

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
	PIPELINE_FLAGS m_flags;
	DNode* m_pParentNode = nullptr;
	std::vector<DConnection*> m_connections;

private:
	CONNECTION_TYPE m_connType;
	std::string m_strName;
};

template <class objType>
class DConnectionTyped : public DConnection {
public:

	DConnectionTyped(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType *pObject, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE) :
		DConnection(pParentNode, strName, connType, optFlags),
		m_pObject(pObject),
		m_ppObject(nullptr)
	{
		// empty
	}

	DConnectionTyped(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType **ppObject, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE) :
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

		// Handle Pass-thru
		if (pTypedOutputConnection->IsPassthru()) {
			// Set the destination?
			CN(pTypedOutputConnection->m_ppObject);
			*pTypedInputConnection->m_ppObject = *pTypedOutputConnection->m_ppObject;
		}
		else {
			// Set the destination?
			CN(pTypedOutputConnection->m_pObject);
			*pTypedInputConnection->m_ppObject = pTypedOutputConnection->m_pObject;
		}


	Error:
		return r;
	}

	RESULT SetConnectionPassthru(DConnectionTyped<objType>* pOutputConnection) {
		*m_ppObject = pOutputConnection->m_pObject;
		return R_PASS;
	}

	RESULT SetConnection(objType **ppObject) {
		m_pObject = *ppObject;
		return R_PASS;
	}

	RESULT SetConnection(objType *pObject) {
		*m_ppObject = pObject;
		return R_PASS;
	}

private:
	objType *m_pObject = nullptr;
	objType **m_ppObject = nullptr;
};


#endif	// ! D_CONNECTION_H_