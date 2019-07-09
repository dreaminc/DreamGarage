#include "TypeObj.h"

TypeObj::TypeObj(TYPE_OBJ type) :
    m_type(type)
{
    /* empty stub */
}

TypeObj::~TypeObj() {
    ACR(Dealloc());
}
