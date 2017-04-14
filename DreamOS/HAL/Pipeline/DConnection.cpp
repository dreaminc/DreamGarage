#include "DConnection.h"

#include "DNode.h"

std::string ConnectionTypeString(CONNECTION_TYPE type) {
	switch (type) {
		case CONNECTION_TYPE::INPUT: return "input"; break;
		case CONNECTION_TYPE::OUTPUT: return "output"; break;
		
		default:
		case CONNECTION_TYPE::INVALID: return "invalid"; break;
	}

	return "invalid";
}

DConnection::DConnection() {
	// empty
}

DConnection::DConnection(std::string strName) :
	m_strName(strName)
{
	// empty
}

DConnection::~DConnection() {
	// empty
}

std::string DConnection::GetName() {
	return m_strName;
}

RESULT DConnection::SetName(std::string strName) {
	m_strName = strName;
	return R_PASS;
}

