#ifndef TEST_OBJECT_H_
#define TEST_OBJECT_H_

// DREAM OS
// DreamOS/Test/TestObject.h
// TestObject is the base class for a given test to be run
// These can be used to benchmark components, or test the system within the
// DreamOS TEST unit testing framework

#include "RESULT/EHM.h"
#include "Test/TestType.h"

RESULT (testCallback *)()

template<typename T> class TestObject {
private:
    T m_result;

public:
    TestObject() {

    }

    ~TestObject() {
        /* stub */
    }

};

#endif // ! TEST_OBJECT_H_
