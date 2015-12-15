#ifndef TEST_OBJECT_H_
#define TEST_OBJECT_H_

// DREAM OS
// DreamOS/Test/TestObject.h
// TestObject is the base class for a given test to be run
// These can be used to benchmark components, or test the system within the
// DreamOS TEST unit testing framework

#include "EHM.h"
#include "Types.h"

// Test callbacks include an argument number and variable arguments 
typedef RESULT fnTest(int, ...);

class TestObject {
private:
    TypeObj *m_pResult;

public:
    TestObject() {
        
    }

    ~TestObject() {
        /* stub */
    }

};

#endif // ! TEST_OBJECT_H_
