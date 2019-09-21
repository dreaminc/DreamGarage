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
	
	RESULT InitializeVulkan();
	RESULT CreateVulkanInstance();

	RESULT SetupVulkanDebugMessenger();
	RESULT PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

	// Vulkan Extensions
	RESULT InitializeVulkanExtensions();
	RESULT RetrieveRequiredVulkanExtensions();
	RESULT RetrieveSupportedVulkanExtensions();

	RESULT CheckValidationLayerSupport();

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
		VkDebugUtilsMessageTypeFlagsEXT msgType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pContext);

	// Physical Devices
	RESULT InitializePhysicalDevices();
	RESULT SelectPhysicalDevice();

private:
// Window Stuff (GLFW)
	GLFWwindow* m_pglfwWindow = nullptr;

	int m_windowWidth = DEFAULT_WINDOW_WIDTH;
	int m_windowHeight = DEFAULT_WINDOW_HEIGHT;

// Vulkan
	VkInstance m_vkInstance;

	
	VkDebugUtilsMessengerEXT m_vulkanDebugMessenger;

	bool m_fValidationLayersEnabled = true;
	const std::vector<const char*> m_vulkanValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	// Vulkan Extensions
	unsigned int m_numSupportedVulkanExtensions = 0;
	std::vector<VkExtensionProperties> m_supportedExtensions;
	std::vector<const char*> m_vulkanRequiredExtensions;

private:
	// Vulkan Extensions
	VkResult VulkanApp::CreateDebugUtilsMessengerEXT(
		VkInstance vkInstance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);

	void DestroyDebugUtilsMessengerEXT(
		VkInstance vkInstance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);

	std::vector<VkPhysicalDevice> m_vkPhysicalDeviceHandles;
	uint32_t m_vkPhysicalDevices_n = 0;
	VkPhysicalDevice m_hVkSelectedPhysicalDevice = nullptr;
};

#endif // ! VULKAN_JUNK_APP_