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


struct VulkanQueueFamilyIndices {
	uint32_t graphicsFamily;
	uint32_t presentFamily;
	bool fVulkanDeviceExtensionsSupported = false;
	
	bool fValid = false;

	static bool CheckVulkanDeviceExtensionSupport(
		VkPhysicalDevice vkDevice, 
		std::vector<const char*> vulkanDeviceRequiredExtensions
	);

	bool IsValid() {
		return fValid && fVulkanDeviceExtensionsSupported;
	}
};

struct VulkanSwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR vkSurfaceCapabilitiesKHR;
	std::vector<VkSurfaceFormatKHR> vkSurfaceFormats;
	std::vector<VkPresentModeKHR> vkPresentModes;

	static VulkanSwapChainSupportDetails QueryVulkanSwapChainSupport(VkPhysicalDevice vkDevice);
};

class VulkanApp {

public:
	VulkanApp();
	~VulkanApp();

	static RESULT TestVulkanSetup();

	RESULT Run();
	RESULT InitializeGLFWWindow();
	RESULT MainLoop();
	RESULT CleanUp();

	// Vulkan
	
	RESULT InitializeVulkan();
	RESULT CreateVulkanInstance();

	RESULT SetupVulkanDebugMessenger();
	RESULT PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

	RESULT CreateVulkanSurface();

	// Vulkan Extensions
	RESULT InitializeVulkanExtensions();
	RESULT RetrieveRequiredVulkanExtensions();
	RESULT RetrieveSupportedVulkanExtensions();

	RESULT CheckValidationLayerSupport();

	// Device Extensions
	RESULT InitializeVulkanDeviceExtensions();
	RESULT RetrieveRequiredVulkanDeviceExtensions();

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
		VkDebugUtilsMessageTypeFlagsEXT msgType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pContext);

	// Physical Devices
	RESULT InitializePhysicalDevices();
	RESULT SelectPhysicalDevice();

	RESULT FindQueueFamilies();

	RESULT InitializeLogicalDevice();

private:
// Window Stuff (GLFW)
	GLFWwindow* m_pglfwWindow = nullptr;

	int m_windowWidth = DEFAULT_WINDOW_WIDTH;
	int m_windowHeight = DEFAULT_WINDOW_HEIGHT;

// Vulkan
	VkInstance m_hVkInstance = nullptr;

	VkDebugUtilsMessengerEXT m_vulkanDebugMessenger;

	bool m_fValidationLayersEnabled = true;
	const std::vector<const char*> m_vulkanValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	// Vulkan Extensions
	unsigned int m_numSupportedVulkanExtensions = 0;
	std::vector<VkExtensionProperties> m_supportedExtensions;
	std::vector<const char*> m_vulkanRequiredExtensions;

	VulkanQueueFamilyIndices m_vulkanQueueFamilyIndices;

	// Vulkan Device Extensions
	std::vector<const char*> m_vulkanDeviceRequiredExtensions;
	

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

	VkApplicationInfo m_vkApplicationInfo = {};
	VkInstanceCreateInfo m_vkInstanceCreateInfo = {};
	VkDeviceQueueCreateInfo m_vkQueueCreateInfo = {};
	VkDeviceCreateInfo m_vkDeviceCreateInfo = {};
	VkPhysicalDeviceFeatures m_vkPhysicalDeviceFeatures = {};

	std::vector<VkPhysicalDevice> m_vkPhysicalDeviceHandles;
	uint32_t m_vkPhysicalDevices_n = 0;
	VkPhysicalDevice m_hVkSelectedPhysicalDevice = nullptr;

	VkDevice m_hVkLogicalDevice = nullptr;
	VkQueue m_hVkGraphicsQueue = nullptr;

	// Presentation
	VkSurfaceKHR m_hVkSurfaceKHR = nullptr;
	VkQueue m_hVkPresentationQueue = nullptr;


};

#endif // ! VULKAN_JUNK_APP_