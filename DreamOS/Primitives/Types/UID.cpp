#include "UID.h"

// Initialize the static value
UINT64 UID::ms_uiIDCounter = (UINT64)(0);

UID::UID() :
	TypeObj(TYPE_OBJ_UID)
{
	this->m_uiID = UID::GetNewUID();
}

RESULT UID::Dealloc() {
	return R_PASS;
}

UINT64 UID::GetID() {
	return this->m_uiID;
}

UINT64 UID::GetNewUID() {
	return (++ms_uiIDCounter);
}