#ifndef TYPE_OBJ_H_
#define TYPE_OBJ_H_

// DREAM OS
// DreamOS/Primitives/TypeObj.h
// TypeObj is the base class for the system defined types like Integer or Matrix
// System types should only be used when there's a very good reason to objectify
// a given variable - as it's a wrapper on top of existing primitives but this
// can be useful when looking to avoid strongly typed paradigms or for scripting
// purposes.

#include "RESULT.h"

typedef enum {
    TYPE_OBJ_NUMBER,
    TYPE_OBJ_STRING,
    TYPE_OBJ_INVALID
} TYPE_OBJ;

class TypeObj {
private:
    TYPE_OBJ m_type;

public:
    TypeObj(TYPE_OBJ type) :
        m_type(type)
    {
        /* empty stub */
    }

    ~TypeObj() {
        if(Dealloc != NULL) {
            ARM(Dealloc());
        }
    }

private:
    virtual RESULT Dealloc() = NULL;
}

#endif // ! TYPE_OBJ_H_
