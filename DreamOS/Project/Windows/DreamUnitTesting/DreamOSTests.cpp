#include "stdafx.h"
#include "CppUnitTest.h"

#include "RESULT/EHM.h"
#include "./../DreamTestApp/DreamTestApp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DreamUnitTesting {		
	TEST_CLASS(DreamOSTests) {
	public:
		
		TEST_METHOD(VaniallaEHMTest) {
			RESULT r = R_PASS;

			// Mock argc/argv 

			const char *argv[] = { "DreamTestApp.exe", "-ts", "hal.quads", "-t", "0" };
			int argc = sizeof(argv) / sizeof(argv[0]);

			DEBUG_LINEOUT("DUT:DOS: Vanilla EHM Test");

			CRM(r, "Vanilla check RESULT code test");

		Error:
			Assert::IsTrue(r == R_PASS);
		}

		TEST_METHOD(VaniallaDOSTest) {
			RESULT r = R_PASS;

			// Mock argc/argv 
			
			const char *argv[] = { "DreamTestApp.exe", "-ts", "hal.quads", "-t", "0" };
			int argc = sizeof(argv) / sizeof(argv[0]);

			DEBUG_LINEOUT("DUT:DOS: Vanilla Test");
			
			{
				DreamTestApp dreamTestApp;
				CRM(dreamTestApp.Initialize(argc, argv), "Failed to initialize Dream Test App");
				CRM(dreamTestApp.Start(), "Failed to start Dream Test App");	// This is the entry point for the DreamOS Engine
			}

		Error:
			Assert::IsTrue(r == R_PASS);
		}

	};
}