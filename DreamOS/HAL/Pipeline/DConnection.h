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

template <class objType>
class DConnectionTyped : public DConnection {
public:
	DConnectionTyped(DNode* pParentNode, CONNECTION_TYPE connType, objType *pDestination);
	DConnectionTyped(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType *pDestination);

private:
	objType *m_pObject = nullptr;
};

class DConnection : public DObject {
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

	RESULT RenderConnections(long frameID = 0);
	RESULT RenderParent(long frameID = 0);

	template <class objType>
	static DConnection* MakeConnection(DNode* pParentNode, std::string strName, CONNECTION_TYPE connType, objType *pObject) {
		RESULT r = R_PASS;

		DConnectionTyped<objType> *pConnectionTyped = nullptr;

		pConnectionTyped = new DConnectionTyped<objType>(pParentNode, strName, connType, pObject);
		CN(pConnectionTyped);

		// Success:
		return pConnectionTyped;

	Error:
		return nullptr;
	}

private:
	DNode* m_pParentNode = nullptr;
	std::vector<DConnection*> m_connections;

private:
	CONNECTION_TYPE m_connType;
	std::string m_strName;
	
};

#endif	// ! D_CONNECTION_H_