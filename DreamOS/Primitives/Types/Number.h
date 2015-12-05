#ifndef INTEGER_H_
#define INTEGER_H_

// DREAM OS
// DreamOS/Primitives/Types/Number.h
// Number is a general purpose dynamic class - ultimately the goal is to allow
// Numbers to work interchangably between templates.

#include "Primitives/Types/TypeObj.h"

typedef enum {
    NUMBER_INT,
    NUMBER_FLOAT,
    NUMBER_DOUBLE,
    NUMBER_INVALID
} NUMBER_TYPE;

typedef enum {
    NUMBER_OPERATION_ADD,
    NUMBER_OPERATION_SUB,
    NUMBER_OPERATION_MULT,
    NUMBER_OPERATION_DIV,
    NUMBER_OPERATION_POW,
    NUMBER_OPERATION_INVALID,
} NUMBER_OPERATION_TYPE;

class Number : public TypeObj {
private:
    void *m_pNumber;

public:
    Number() :
        super(TYPE_OBJ_NUMBER),
        m_pNumber(NULL)
    {
        ACR(allocNumberType(0, NUMBER_INVALID));
    }

    Number(int val) :
        super(TYPE_OBJ_NUMBER),
        m_pNumber(NULL)
    {
        ACR(allocNumberType((void*)&val, NUMBER_INT));
    }

    Number(float val) :
        super(TYPE_OBJ_NUMBER),
        m_pNumber(NULL)
    {
        ACR(allocNumberType((void*)&val, NUMBER_FLOAT));
    }

    Number(double val) :
        super(TYPE_OBJ_NUMBER),
        m_pNumber(NULL)
    {
        ACR(allocNumberType((void*)&val, NUMBER_DOUBLE));
    }

    // Destructor implied

private:
    // Dual purpose get and cast functions
    inline int getInt() { return (int)(*(int *)this->m_pNumber);}
    inline float getFloat() { return (float)(*(float *)this->m_pNumber); }
    inline double getDouble() { return (double)(*(double *)this->m_pNumber); }

    inline void setInt(int val) {
        this->m_pNumber = val;
    }

    inline void setInt(void *pVal) {
        ACNM(pVal, "Int ptr cannot be NULL");
        this->m_pNumber = *(int *)pVal;
    }

    inline void setFloat(float val) {
        this->m_pNumber = val;
    }

    inline void setFloat(void *pVal) {
        ACNM(pVal, "Float ptr cannot be NULL");
        this->m_pNumber = *(float *)pVal;
    }

    inline void setDouble(double val) {
        this->m_pNumber = val;
    }

    inline void setDouble(void *pVal) {
        ACNM(pVal, "Double ptr cannot be NULL");
        this->m_pNumber = *(double *)pVal;
    }

    inline RESULT AddNumber(Number num) {
        switch(m_type) {
            case NUMBER_INT: {

            } break;

            case NUMBER_FLOAT: {
                this->m_pNumber = new float(*(float *)pVal);
                CNM(this->m_pNumber, "Failed to alloc float Number");
            } break;

            case NUMBER_DOUBLE: {
                this->m_pNumber = new double(*(double *)pVal);
                CNM(this->m_pNumber, "Failed to alloc double Number");
            } break;
        }

        return R_OK;
    }

    // This is a RHS operation function:
    // LHS OP RHS
    RESULT operateNumber(Number num, NUMBER_OPERATION_TYPE type) {
        RESULT r = R_OK;

        switch(m_type) {
            case NUMBER_INT: {

            } break;

            case NUMBER_FLOAT: {
                this->m_pNumber = new float(*(float *)pVal);
                CNM(this->m_pNumber, "Failed to alloc float Number");
            } break;

            case NUMBER_DOUBLE: {
                this->m_pNumber = new double(*(double *)pVal);
                CNM(this->m_pNumber, "Failed to alloc double Number");
            } break;
        }

    Error:
        return r;
    }

private:
    RESULT allocNumberType(void *pVal, NUMBER_TYPE type) {
        RESULT r = R_OK;

        CBM((type != NUMBER_INVALID), "Type cannot be invalid");
        CNM(pVal, "Value cannot be NULL");

        switch(type) {
            case NUMBER_INT: {
                this->m_pNumber = new int;
                CNM(this->m_pNumber, "Failed to alloc int Number");
                setInt(pVal);
            } break;

            case NUMBER_FLOAT: {
                this->m_pNumber = new float;
                CNM(this->m_pNumber, "Failed to alloc float Number");
                setFloat(pVal);
            } break;

            case NUMBER_DOUBLE: {
                this->m_pNumber = new double;
                CNM(this->m_pNumber, "Failed to alloc double Number");
                setDouble(pVal);
            } break;
        }

    Error:
        return r;
    }

// Operators
public:
    // Copy
    Number& operator=(const Number& arg) {
        m_number = arg.m_number;
        return *this;
    }

    // Move
    Number& operator=(Number&& arg) {
        ACB(this != &arg);   // TODO: Asserts / EHM
        m_number = arg.m_number;
        return *this;
    }

    // Comparison
    bool operator==( const Number<T> &arg) const {
        return m_number == arg.m_number;
    }

    bool operator==( const T &arg) const {
        return m_number == arg;
    }

    // Add
    Number<T>& operator+=(const Number<T> &rhs) {
        m_number += rhs.m_number;
        return *this;
    }

    Number<T>& operator+=(const T &rhs) {
        m_int += rhs;
        return *this;
    }

    friend Number<T> operator+(Number<T> lhs, const Number<T> &rhs) {
        lhs += rhs; // Reuse compound assignment
        return lhs;
    }

    friend Number<T> operator+(Number<T> lhs, const T &rhs) {
        lhs += rhs; // Reuse compound assignment
        return lhs;
    }

    Number<T> operator+( const Number<T> &arg ) const {
        return Number<T>(*this).operator+=(arg);
    }

    Number<T> operator+( const T &arg ) const {
        return Number(*this).operator+=(arg);
    }

private:
    // No real Destructor
    RESULT Dealloc() = NULL;
}

#endif // !INTEGER_H_
