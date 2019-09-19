#ifndef VULKAN_JUNK_APP_
#define VULKAN_JUNK_APP_

#include "core/ehm/EHM.h"

// Dream Vulkan Junk App
// dos/src/junk/vulkan/VulkanApp.h

// #include <vulkan/vulkan.h>  // Don't need to add this if using GLFW
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

class VulkanApp {

public:
	VulkanApp();
	~VulkanApp();

	static RESULT TestVulkanSetup();

	RESULT Run();
	RESULT InitWindow();
	RESULT MainLoop();
	RESULT CleanUp();

	// Vulkan
	
	RESULT InitVulkan();
	RESULT CreateVulkanInstance();
	RESULT RetrieveSupportedVulkanExtensions(const char** ppszGLFWExtensions = nullptr, unsigned int numExtensions = 0);

private:
// Window Stuff (GLFW)
	GLFWwindow* m_pglfwWindow = nullptr;

	int m_windowWidth = DEFAULT_WINDOW_WIDTH;
	int m_windowHeight = DEFAULT_WINDOW_HEIGHT;

// Vulkan
	VkInstance m_vkInstance;

	unsigned int m_numSupportedVulkanExtensions = 0;
	std::vector<VkExtensionProperties> m_supportedExtensions;

};

#endif // ! VULKAN_JUNK_APP_