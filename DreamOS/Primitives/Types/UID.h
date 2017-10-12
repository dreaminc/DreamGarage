#ifndef UID_H_
#define UID_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/TypeObj.h"

class UID : public TypeObj {
public: 
	UID();

	UINT64 GetID();
	RESULT Dealloc();

	bool operator==(const UID& rhs) {
		return(m_uiID == rhs.m_uiID);
	}

	bool operator<(const UID& rhs) {
		return(m_uiID < rhs.m_uiID);
	}

	friend inline bool operator==(const UID& lhs, const UID& rhs) {
		return(lhs.m_uiID == rhs.m_uiID);
	}

	friend inline bool operator<(const UID& lhs, const UID& rhs) {
		return(lhs.m_uiID < rhs.m_uiID);
	}

private:
	UINT64 m_uiID;

private:
	static UINT64 GetNewUID();

	// Initialized in source
	static UINT64 ms_uiIDCounter;
};

#endif // ! UID_H_
