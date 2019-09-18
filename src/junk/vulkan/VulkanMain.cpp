#include "VulkanMain.h"
#include "VulkanApp.h"

#include <string>

#ifdef _WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	RESULT r = R_PASS;

	// Get command line args and put them on the stack in argc,argv format
	LPWSTR* wargv = nullptr;
	int argc = 0;
	VulkanApp* pVulkanApp = nullptr;

	wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
	char** argv = new char* [argc];
	std::vector<std::string> args;

	// Set up a console and attach to it
	// TODO: Might want to explore this more (add it explicitly)
#ifdef _DEBUG
	if (AllocConsole()) {

		AttachConsole(GetCurrentProcessId());

		freopen("CONOUT$", "wt", stdout);
		freopen("CONOUT$", "wt", stderr);

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	}
	else {
		CBM(false, "Failed to allocate console");
	}
#endif

	for (int i = 0; i < argc; i++) {
		std::wstring warg(wargv[i]);
		args.push_back(std::string(warg.begin(), warg.end()));
	}

	// now argc,argv are available and will get destroyed on exit
	for (int i = 0; i < argc; i++) {
		argv[i] = new char;
		argv[i] = (char*)args[i].c_str();
	}

	//CRM(VulkanApp::TestVulkanSetup(), "Vulkan Test Setup Failed");
	
	pVulkanApp = new VulkanApp();
	CNM(pVulkanApp, "Failed to allocate Vulkan App");

	CRM(pVulkanApp->Run(), "Vulkan Test Setup Failed");

Success:
	if (pVulkanApp != nullptr) {
		delete pVulkanApp;
		pVulkanApp = nullptr;
	}

	return (int)(r);

Error:
	if (pVulkanApp != nullptr) {
		delete pVulkanApp;
		pVulkanApp = nullptr;
	}

	DEBUG_LINEOUT("Junk Vulkan Project Exiting with Error 0x%x result", r);
	DEBUG_SYSTEM_PAUSE();

	return (int)(r);
}

#else

int main(int argc, const char* argv[]) {
	RESULT r = R_PASS;
	VulkanApp* pVulkanApp;

	//CRM(VulkanApp::TestVulkanSetup(), "Vulkan Test Setup Failed");
	pVulkanApp = new VulkanApp();
	CNM(pVulkanApp, "Failed to allocate Vulkan App");

	CRM(vulkanApp.Run(), "Vulkan Test Setup Failed");

Success:
	if (pVulkanApp != nullptr) {
		delete pVulkanApp;
		pVulkanApp = nullptr;
	}

	DEBUG_LINEOUT("Junk Vulkan Project Exiting");
	return (int)(r);

Error:
	if (pVulkanApp != nullptr) {
		delete pVulkanApp;
		pVulkanApp = nullptr;
	}

	DEBUG_LINEOUT("Junk Vulkan Project exiting with Error 0x%x result", r);
	system("pause");

	return (int)(r);
}
#endif