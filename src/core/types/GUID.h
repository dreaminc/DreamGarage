#ifndef GUID_H_
#define GUID_H_

#include "core/ehm/EHM.h"
#include "core/types/TypeObj.h"

// DREAM OS
// DreamOS/Cloud/core/types/GUID.h
// GUID is a globally unique identifier, different than UID in that UID is only unqiue to the
// Dream OS landscape.

#include <string>

// TODO: This is the Windows definition of GUID and is being added
// here for the purpose of compilation (included in <windows.h>

typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;

class guid : public TypeObj {
public:
	guid();

	//UINT64 GetID();
	//RESULT Dealloc();

	std::string GetGUIDString();

	RESULT SetGUIDFromString(std::string strGUID);

	bool operator==(const guid& rhs) {
		return ((m_uiIDUpper == rhs.m_uiIDUpper) && (m_uiIDLower == rhs.m_uiIDLower));
	}

	friend inline bool operator==(const guid& lhs, const guid& rhs) {
		return ((lhs.m_uiIDUpper == rhs.m_uiIDUpper) && (lhs.m_uiIDLower == rhs.m_uiIDLower));
	}

private:
	UINT64 m_uiIDUpper;
	UINT64 m_uiIDLower;

private:
	static RESULT IncrementUID();
	static RESULT GetNewUID(UINT64 *puiIDCounterLower, UINT64 *puiIDCounterUpper);

	// Initialized in source
	static UINT64 ms_uiIDCounterLower;
	static UINT64 ms_uiIDCounterUpper;
};

#endif // ! GUID_H_
