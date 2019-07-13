#ifndef INTEGER_H_
#define INTEGER_H_

#include <math.h>

// DREAM OS
// DreamOS/Primitives/Types/Number.h
// Number is a general purpose dynamic class - ultimately the goal is to allow
// Numbers to work interchangably between templates.

#include "TypeObj.h"

typedef enum {
    NUMBER_INT,
    NUMBER_FLOAT,
    NUMBER_DOUBLE,
    // TODO: Add NAN, POSINF, NEGINF, E, PI, others?
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
    NUMBER_TYPE m_numberType;

public:
    Number() :
        TypeObj(TYPE_OBJ_NUMBER),
        m_pNumber(NULL)
    {
        ACR(allocNumberType(0, NUMBER_INVALID));
    }

    Number(int val) :
        TypeObj(TYPE_OBJ_NUMBER),
        m_pNumber(NULL)
    {
        ACR(allocNumberType((void*)&val, NUMBER_INT));
    }

    Number(float val) :
        TypeObj(TYPE_OBJ_NUMBER),
        m_pNumber(NULL)
    {
        ACR(allocNumberType((void*)&val, NUMBER_FLOAT));
    }

    Number(double val) :
        TypeObj(TYPE_OBJ_NUMBER),
        m_pNumber(NULL)
    {
        ACR(allocNumberType((void*)&val, NUMBER_DOUBLE));
    }
    
    // Copy constructor
    Number(const Number &num) :
        TypeObj(TYPE_OBJ_NUMBER),
        m_pNumber(NULL)
    {
        ACR(allocNumberType((void*)(&num.m_pNumber), num.m_numberType));
    }

    // Destructor implied
    
    RESULT Dealloc() {
        if(this->m_pNumber != NULL) {
            free(this->m_pNumber);
            this->m_pNumber = NULL;
        }
        
        return R_PASS;
    }
    
public:
    NUMBER_TYPE getNumberType() {
        return this->m_numberType;
    }

protected:
    // Dual purpose get and cast functions
    int getInt() { return (int)(*(int *)this->m_pNumber);}
    float getFloat() { return (float)(*(float *)this->m_pNumber); }
    double getDouble() { return (double)(*(double *)this->m_pNumber); }

private:
    void setInt(int val) { *(int*)this->m_pNumber = val; }
    void setInt(void *pVal) {
        ACNM(pVal, "Int ptr cannot be NULL");
        *(int*)this->m_pNumber = *(int *)pVal;
    }

    void setFloat(float val) { *(float*)this->m_pNumber = val; }
    void setFloat(void *pVal) {
        ACNM(pVal, "Float ptr cannot be NULL");
        *(float*)this->m_pNumber = *(float *)pVal;
    }

    void setDouble(double val) { *(double*)this->m_pNumber = val; }
    void setDouble(void *pVal) {
        ACNM(pVal, "Double ptr cannot be NULL");
        *(double*)this->m_pNumber = *(double *)pVal;
    }
    
    // TODO: Need to cover all of the cases
    RESULT setNumber(Number num) {
        switch(num.m_numberType) {
            case NUMBER_INT: setInt(num.getInt()); break;
            case NUMBER_FLOAT: setFloat(num.getFloat()); break;
            case NUMBER_DOUBLE: setDouble(num.getDouble()); break;
                
            case NUMBER_INVALID:
            default: return R_FAIL;
        }
        
        return R_PASS;
    }

    void AddInt(int rhs) { *(int*)this->m_pNumber += rhs; }
    void AddFloat(float rhs) { *(float*)this->m_pNumber += rhs; }
    void AddDouble(double rhs) { *(double*)this->m_pNumber += rhs; }
    
    inline RESULT AddNumber(Number num) {
        switch(m_numberType) {
            case NUMBER_INT: setInt(getInt() + num.getInt()); break;
            case NUMBER_FLOAT: setFloat(getFloat() + num.getFloat()); break;
            case NUMBER_DOUBLE: setDouble(getDouble() + num.getDouble()); break;
                
            case NUMBER_INVALID:
            default: return R_FAIL;
        }

        return R_PASS;
    }
    
    const bool CompareInt(int rhs) { return (*(int*)this->m_pNumber) == rhs; }
    const bool CompareFloat(float rhs) { return (*(float*)this->m_pNumber) == rhs; }
    const bool CompareDouble(double rhs) { return (*(double*)this->m_pNumber) == rhs; }

private:
    RESULT allocNumberType(void *pVal, NUMBER_TYPE type) {
        RESULT r = R_PASS;

        CBM((type != NUMBER_INVALID), "Type cannot be invalid");
        CNM(pVal, "Value cannot be NULL");

        switch(type) {
            case NUMBER_INT: {
                this->m_pNumber = (int*)malloc(sizeof(int));
                CNM(this->m_pNumber, "Failed to alloc int Number");
                setInt(pVal);
            } break;

            case NUMBER_FLOAT: {
                this->m_pNumber = (float*)malloc(sizeof(float));
                CNM(this->m_pNumber, "Failed to alloc float Number");
                setFloat(pVal);
            } break;

            case NUMBER_DOUBLE: {
                this->m_pNumber = (double*)malloc(sizeof(double));
                CNM(this->m_pNumber, "Failed to alloc double Number");
                setDouble(pVal);
            } break;
                
            case NUMBER_INVALID:
            default: return R_FAIL;
        }

    Error:
        return r;
    }

// Operators
public:
    // Copy
    Number& operator=(const Number& arg) {
        this->setNumber(arg);
        return *this;
    }

    // Move
    Number& operator=(Number&& arg) {
        this->setNumber((Number)(arg));
        return *this;
    }

    // Comparison
    bool operator==(const Number &arg) const {
        return m_pNumber == arg.m_pNumber;
    }
    
    //const bool operator==(const int &rhs) const { return CompareInt(rhs); }
    //bool operator==(const float &rhs) const { return this->CompareFloat(rhs); }
    //bool operator==(const double &rhs) const { return this->CompareDouble(rhs); }

    // Add
    Number& operator+=(const Number &rhs) {
        this->AddNumber(rhs);
        return *this;
    }
    
    Number& operator+=(const int &rhs) {
        this->AddInt(rhs);
        return *this;
    }
    
    Number& operator+=(const float &rhs) {
        this->AddFloat(rhs);
        return *this;
    }
    
    Number& operator+=(const double &rhs) {
        this->AddDouble(rhs);
        return *this;
    }

    friend Number operator+(Number lhs, const Number &rhs) {
        lhs += rhs; // Reuse compound assignment
        return lhs;
    }
    
    friend Number operator+(Number lhs, const int &rhs) { lhs += rhs; return lhs; }// Reuse compound assignment
    friend Number operator+(Number lhs, const float &rhs) { lhs += rhs; return lhs; }// Reuse compound assignment
    friend Number operator+(Number lhs, const double &rhs) { lhs += rhs; return lhs; }// Reuse compound assignment

    Number operator+( const Number &arg ) const {
        return Number(*this).operator+=(arg);
    }
    
    Number operator+( const int &arg ) const { return Number(*this).operator+=(arg); }
    Number operator+( const float &arg ) const { return Number(*this).operator+=(arg); }
    Number operator+( const double &arg ) const { return Number(*this).operator+=(arg); }

	// Utility
public:
	static long DigitCount(double num) { 
		if (num == 0)
			return 1;

		long digitCount = (long)ceil(log10((double)(num)));
		return digitCount;
	}
};

#endif // !INTEGER_H_
