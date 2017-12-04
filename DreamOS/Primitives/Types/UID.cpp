#include "UID.h"

// Initialize the static value
UINT64 UID::ms_uiIDCounter = (UINT64)(0);

UID::UID() :
	TypeObj(TYPE_OBJ_UID)
{
	this->m_uiID = UID::GetNewUID();
}

UID::UID(UID::type uidType) :
	TypeObj(TYPE_OBJ_UID)
{
	switch (uidType) {
		case type::VALID: {
			this->m_uiID = UID::GetNewUID();
			m_fValid = true;
		}break;
	
		case type::INVALID: 
		default: {
			m_fValid = false;
			this->m_uiID = 0;
		} break;
	}
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

UID UID::MakeInvalidUID() {
	UID newUID(type::INVALID);
	return newUID;
}

bool UID::IsValid() {
	return m_fValid;
}