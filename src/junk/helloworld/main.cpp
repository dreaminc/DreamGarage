#include <stdio.h>
#include <cstdlib>
#include <iostream>

#include <cstdio>

// Ensure EHM
#include "core/ehm/EHM.h"

struct DummyStruct {
    int a = 5;
};

RESULT TestFunction() {
    RESULT r = R_PASS;

    DummyStruct dummyStruct;

    char szTestString[250];
    memset(szTestString, 0, sizeof(szTestString));

    CBM(false, "This is designed to fail");

    DummyStruct *pDummyStruct;
    pDummyStruct = new DummyStruct();

    DEBUG_LINEOUT("DummyString.a = %d pDummyStruct->a = %d", dummyStruct.a, pDummyStruct->a);

Error:
    return r;
}

int main(int argc, char **argv) {

    DEBUG_LINEOUT("Hello World!");

    //printf("Hello World!\n");

    printf("Arguments: \n");

    for(int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    TestFunction();

#ifdef __APPLE__
    //std::cin.ignore(10000);
    //std::cin.get();
#else
    system("pause");
#endif

    printf("exiting...");
}

