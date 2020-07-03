#include "VulkanApp.h"

#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#include <set>

VulkanApp::VulkanApp() {
	// 
}

VulkanApp::~VulkanApp() {
	// 
}
 
RESULT VulkanApp::TestVulkanSetup() {
	RESULT r = R_PASS;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* pglfwWindow = glfwCreateWindow(800, 600, "Vulkan Test Window", nullptr, nullptr);

	unsigned int extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	DEBUG_LINEOUT("%d extensions supported", extensionCount);

	glm::mat4 matrix;
	glm::vec4 vec;

	glm::vec4 test = matrix * vec;

	while (!glfwWindowShouldClose(pglfwWindow)) {
		glfwPollEvents();
	}

Error:
	if (pglfwWindow != nullptr) {
		glfwDestroyWindow(pglfwWindow);
		pglfwWindow = nullptr;
	}
	
	glfwTerminate();

	return r;
}

RESULT VulkanApp::Run() {
	RESULT r = R_PASS;

	CRM(InitializeGLFWWindow(), "Vulkan initialization failed");

	CRM(InitializeVulkan(), "Vulkan initialization failed");

	CRM(MainLoop(), "Main Loop Error");

	CRM(CleanUp(), "Clean up failed");

Error:
	return r;
}

RESULT VulkanApp::InitializeGLFWWindow() {
	RESULT r = R_PASS;

	glfwInit();

	// No GL context 
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);		// no resizing for now

	m_pglfwWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, "Vulkan Junk App", nullptr, nullptr);
	CNM(m_pglfwWindow, "Failed to create GLFW window")

Error:
	return r;
}

RESULT VulkanApp::InitializeVulkanExtensions() {
	RESULT r = R_PASS;

	CRM(RetrieveRequiredVulkanExtensions(), "Failed to retrieve required vulkan extensions");

	CRM(RetrieveSupportedVulkanExtensions(), "Failed to retrieve supproted vulkan extensions");

Error:
	return r;
}

RESULT VulkanApp::RetrieveRequiredVulkanExtensions() {
	RESULT r = R_PASS;

	// TODO: Related to GLFW vs. WIN32 API 
	// We will need to figure out what extensions Dream needs

	uint32_t glfwExtensionCount = 0;
	const char** ppszGLFWExtensions;

	ppszGLFWExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	CNM(ppszGLFWExtensions, "Failed to retrieve GLFW extensions");

	for(unsigned int i = 0; i < glfwExtensionCount; i++) {
		DEBUG_LINEOUT("Required extension added: %s", ppszGLFWExtensions[i]);
		m_vulkanRequiredExtensions.push_back(ppszGLFWExtensions[i]);
	}

	if (m_fValidationLayersEnabled) {
		m_vulkanRequiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

Error:
	return r;
}

RESULT VulkanApp::RetrieveSupportedVulkanExtensions() {
	RESULT r = R_PASS;

	// TODO: Related to GLFW vs. WIN32 API 
	// We will need to figure out what extensions Dream needs

	// Retrieve supported extensions
	vkEnumerateInstanceExtensionProperties(nullptr, &m_numSupportedVulkanExtensions, nullptr);
	m_supportedExtensions = std::vector<VkExtensionProperties>(m_numSupportedVulkanExtensions);

	vkEnumerateInstanceExtensionProperties(nullptr, &m_numSupportedVulkanExtensions, m_supportedExtensions.data());
	
	for (const auto& extension : m_supportedExtensions) {
		DEBUG_LINEOUT("vk-ext found: %s", extension.extensionName);
	}

	if (m_vulkanRequiredExtensions.size() > 0) {
		for (const auto& pszRequiredVkExt : m_vulkanRequiredExtensions) {
			bool fFound = false;

			for (const auto& extension : m_supportedExtensions) {
				if (strcmp(extension.extensionName, pszRequiredVkExt) == 0) {
					fFound = true;
				}
			}

			CBM(fFound, "Error: vulkan extension %s not supported", pszRequiredVkExt);

			DEBUG_LINEOUT("Found extension %s", pszRequiredVkExt);
		}
	}

Error:
	return r;
}

RESULT VulkanApp::CheckValidationLayerSupport() {
	RESULT r = R_PASS;

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> vulkanAvailableValidationLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, vulkanAvailableValidationLayers.data());

	for (const char* pszLayerName : m_vulkanValidationLayers) {
		bool fFound = false;

		for (const auto& layerProperties : vulkanAvailableValidationLayers) {
			if (strcmp(pszLayerName, layerProperties.layerName) == 0) {
				fFound = true;
				break;
			}
		}

		CBM(fFound, "Vulkan validation layer %s not found", pszLayerName);
	}

Error:
	return r;
}

RESULT VulkanApp::InitializeVulkanDeviceExtensions() {
	RESULT r = R_PASS;

	CRM(RetrieveRequiredVulkanDeviceExtensions(), "Failed to retrieve required vulkan device extensions");

Error:
	return r;
}

RESULT VulkanApp::RetrieveRequiredVulkanDeviceExtensions() {
	RESULT r = R_PASS;

	m_vulkanDeviceRequiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

Error:
	return r;
}

/*
RESULT VulkanApp::RetrieveSupportedVulkanDeviceExtensions() {
	RESULT r = R_PASS;

	// TODO: Related to GLFW vs. WIN32 API 
	// We will need to figure out what extensions Dream needs

	// Retrieve supported extensions
	vkEnumerateInstanceExtensionProperties(nullptr, &m_numSupportedVulkanExtensions, nullptr);
	m_supportedExtensions = std::vector<VkExtensionProperties>(m_numSupportedVulkanExtensions);

	vkEnumerateInstanceExtensionProperties(nullptr, &m_numSupportedVulkanExtensions, m_supportedExtensions.data());

	for (const auto& extension : m_supportedExtensions) {
		DEBUG_LINEOUT("vk-ext found: %s", extension.extensionName);
	}

	if (m_vulkanRequiredExtensions.size() > 0) {
		for (const auto& pszRequiredVkExt : m_vulkanRequiredExtensions) {
			bool fFound = false;

			for (const auto& extension : m_supportedExtensions) {
				if (strcmp(extension.extensionName, pszRequiredVkExt) == 0) {
					fFound = true;
				}
			}

			CBM(fFound, "Error: vulkan extension %s not supported", pszRequiredVkExt);

			DEBUG_LINEOUT("Found extension %s", pszRequiredVkExt);
		}
	}

Error:
	return r;
}
*/


RESULT VulkanApp::CreateVulkanInstance() {
	RESULT r = R_PASS;

	// App Info
	m_vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	m_vkApplicationInfo.pApplicationName = "Hello Triangle";
	m_vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	m_vkApplicationInfo.pEngineName = "No engine";
	m_vkApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	m_vkApplicationInfo.apiVersion = VK_API_VERSION_1_0;

	// Create Info
	m_vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	m_vkInstanceCreateInfo.pApplicationInfo = &m_vkApplicationInfo;

	// Extensions
	CRM(InitializeVulkanExtensions(), "Failed to initialize vulkan extensions");
	m_vkInstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_vulkanRequiredExtensions.size());
	m_vkInstanceCreateInfo.ppEnabledExtensionNames = m_vulkanRequiredExtensions.data();

	// Enable validation support if our layers are supported
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (m_fValidationLayersEnabled) {
		CRM(CheckValidationLayerSupport(), "Vulkan validation layer check failed");

		m_vkInstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_vulkanValidationLayers.size());
		m_vkInstanceCreateInfo.ppEnabledLayerNames = m_vulkanValidationLayers.data();

		CRM(PopulateDebugMessengerCreateInfo(debugCreateInfo), 
			"Failed to populate debug messenger info for instance creation");

		m_vkInstanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)(&debugCreateInfo);
	}
	else {
		m_vkInstanceCreateInfo.enabledLayerCount = 0;
		m_vkInstanceCreateInfo.pNext = nullptr;
	}

	// Create our instance
	CBM(vkCreateInstance(&m_vkInstanceCreateInfo, nullptr, &m_hVkInstance) == VK_SUCCESS, "Failed to create VK instance");

Error:
	return r;
}

// Should go into an extension manager like OGLImp has in dos/hal
// Proxy function for extension
VkResult VulkanApp::CreateDebugUtilsMessengerEXT(
	VkInstance vkInstance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	RESULT r = R_PASS;

	auto pfnVkCreateDebugUtilsMessengerEXT =
		(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");

	CNM(pfnVkCreateDebugUtilsMessengerEXT, "vkCreateDebugUtilsMessengerEXT not present");

Success:
	return pfnVkCreateDebugUtilsMessengerEXT(vkInstance, pCreateInfo, pAllocator, pDebugMessenger);

Error:
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanApp::DestroyDebugUtilsMessengerEXT(
	VkInstance vkInstance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	RESULT r = R_PASS;

	auto pfnVkDestroyDebugUtilsMessengerEXT =
		(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");

	CNM(pfnVkDestroyDebugUtilsMessengerEXT, "vkDestroyDebugUtilsMessengerEXT not present");

Success:
	return pfnVkDestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, pAllocator);

Error:
	return;
}

RESULT VulkanApp::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
	RESULT r = R_PASS;

	createInfo = {};

	CBM(m_fValidationLayersEnabled, "Cannot set up debug messenger if validation layers are disabled");

	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	createInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		//VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT;

	createInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		//VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT;

	createInfo.pfnUserCallback = VulkanDebugCallback;
	createInfo.pUserData = (void*)(this);

Error:
	return r;
}

RESULT VulkanApp::SetupVulkanDebugMessenger() {
	RESULT r = R_PASS;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};

	CBM(m_fValidationLayersEnabled, "Cannot set up debug messenger if validation layers are disabled");

	CRM(PopulateDebugMessengerCreateInfo(createInfo), "Failed to populate vkDebugUtilsMessengerCreateInfoEXT");

	// Set up the call backs for the debug messenger
	CBM((CreateDebugUtilsMessengerEXT(m_hVkInstance, &createInfo, nullptr, &m_vulkanDebugMessenger) == VK_SUCCESS),
		"Failed to set up vulkan debug messenger");

Error:
	return r;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanApp::VulkanDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
	VkDebugUtilsMessageTypeFlagsEXT msgType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pContext)
{
	RESULT r = R_PASS;

	VulkanApp* pApp = reinterpret_cast<VulkanApp*>(pContext);
	CNM(pApp, "Invalid VulkanApp in user data");

	switch (msgSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: 
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
			// Ignore these right now
		} break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: {
			DEBUG_LINEOUT("Validation Layer: %s", pCallbackData->pMessage);
		} break;
	}

Success:
	return VK_TRUE;

Error:
	return VK_FALSE;
}

RESULT VulkanApp::InitializeVulkan() {
	RESULT r = R_PASS;

	// Disable validation layers for debug builds
#ifndef _DEBUG
	bool m_fValidationLayersEnabled = false;
#endif

	CRM(CreateVulkanInstance(), "Failed to create vulkan instance");

	if (m_fValidationLayersEnabled) {
		CRM(SetupVulkanDebugMessenger(), "Failed to set up vulkan debug messenger");
	}

	CRM(CreateVulkanSurface(), "Failed to create vulkan surface")

	CRM(InitializePhysicalDevices(), "Failed to initialize physical devices");

	CRM(InitializeLogicalDevice(), "Failed to initialize logical device");

Error:
	return r;
}

// TODO: More sophisticated device selection process
// TODO: Could also use multiple devices
bool IsVulkanDeviceSuitable(VkPhysicalDevice vkDeviceHandle) {
	RESULT r = R_PASS;

	bool fSuitable = false;

	VkPhysicalDeviceProperties vkPhysicalDeviceProperties; 
	VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures;

	vkGetPhysicalDeviceProperties(vkDeviceHandle, &vkPhysicalDeviceProperties);
	vkGetPhysicalDeviceFeatures(vkDeviceHandle, &vkPhysicalDeviceFeatures);

	DEBUG_LINEOUT("Found device: %s", vkPhysicalDeviceProperties.deviceName);

	// Discrete GPU
	if (vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		vkPhysicalDeviceFeatures.geometryShader == VK_TRUE &&
		vkPhysicalDeviceFeatures.tessellationShader == VK_TRUE
		)
	{
		fSuitable = true;
	}

	// Integrated GPU
	if (vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU &&
		vkPhysicalDeviceFeatures.geometryShader == VK_TRUE &&
		vkPhysicalDeviceFeatures.tessellationShader == VK_TRUE
		)
	{
		fSuitable = true;
	}

Success:
	return fSuitable;

Error:
	return false;
}

// TODO: Only windows
#include "vulkan/vulkan_win32.h"

// Needed for GLFW Win32 Specific functions
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

RESULT VulkanApp::CreateVulkanSurface() {
	RESULT r = R_PASS;

	// Win32 way
	/*
	VkWin32SurfaceCreateInfoKHR vkWin32SuraceCreateInfoKHR = {};
	vkWin32SuraceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	vkWin32SuraceCreateInfoKHR.hwnd = glfwGetWin32Window(m_pglfwWindow);
	vkWin32SuraceCreateInfoKHR.hinstance = GetModuleHandle(nullptr);

	CBM((vkCreateWin32SurfaceKHR(m_hVkInstance, &vkWin32SuraceCreateInfoKHR, nullptr, &m_vkSurfaceKHR) == VK_SUCCESS),
		"Failed to create Win32 vulkan surface KHR");
	*/

	CBM((glfwCreateWindowSurface(m_hVkInstance, m_pglfwWindow, nullptr, &m_hVkSurfaceKHR) == VK_SUCCESS),
		"Failed to create Win32 vulkan surface KHR using GLFW");


Error:
	return r;
}

RESULT VulkanApp::InitializePhysicalDevices() {
	RESULT r = R_PASS;

	vkEnumeratePhysicalDevices(m_hVkInstance, &m_vkPhysicalDevices_n, nullptr);
	CBM((m_vkPhysicalDevices_n > 0), "No vulkan physical devices found");

	m_vkPhysicalDeviceHandles = std::vector<VkPhysicalDevice>(m_vkPhysicalDevices_n);
	vkEnumeratePhysicalDevices(m_hVkInstance, &m_vkPhysicalDevices_n, m_vkPhysicalDeviceHandles.data());

	for (const auto& vkDeviceHandle : m_vkPhysicalDeviceHandles) {
		if (IsVulkanDeviceSuitable(vkDeviceHandle)) {
			if (m_hVkSelectedPhysicalDevice == nullptr) {

				uint32_t queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(vkDeviceHandle, &queueFamilyCount, nullptr);
				std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(vkDeviceHandle, &queueFamilyCount, queueFamilies.data());

				int i = 0;
				bool fFoundValidQueueFamily = false;
				for (const auto& queueFamily : queueFamilies) {
					if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
						fFoundValidQueueFamily = true;
						break;
					}
				}
				
				// Only set if we find a valid queue family
				if (fFoundValidQueueFamily) {
					m_hVkSelectedPhysicalDevice = vkDeviceHandle;
				}
			}
		}
	}

	VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(m_hVkSelectedPhysicalDevice, &vkPhysicalDeviceProperties);
	DEBUG_LINEOUT("Selected device: %s", vkPhysicalDeviceProperties.deviceName);

	CNM(m_hVkSelectedPhysicalDevice, "Failed to find suitable vk physical device");

	// Check queue families (sets the struct)
	CRM(FindQueueFamilies(), "Failed to find queue families for selected vk physical device");
	CBM(m_vulkanQueueFamilyIndices.IsValid(), "Error: invalid queue family index");

Error:
	return r;
}

RESULT VulkanApp::SelectPhysicalDevice() {
	RESULT r = R_PASS;

	// TODO: ??

Error:
	return r;
}

// Check the extensions match for a device per required extensions
bool VulkanQueueFamilyIndices::CheckVulkanDeviceExtensionSupport(VkPhysicalDevice vkDevice, std::vector<const char*> vulkanDeviceRequiredExtensions) {
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(vkDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(vkDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(vulkanDeviceRequiredExtensions.begin(), vulkanDeviceRequiredExtensions.end());

	for (const auto& vkExtension : availableExtensions) {
		requiredExtensions.erase(vkExtension.extensionName);
	}

	return requiredExtensions.empty();
}

// TODO: Actual implementation should be a bit more robust than this
RESULT VulkanApp::FindQueueFamilies() {
	RESULT r = R_PASS;

	uint32_t queueFamilyCount = 0;
	std::vector<VkQueueFamilyProperties> queueFamilies;

	CNM(m_hVkSelectedPhysicalDevice, "No physical vulkan device selected");

	vkGetPhysicalDeviceQueueFamilyProperties(m_hVkSelectedPhysicalDevice, &queueFamilyCount, nullptr);

	queueFamilies.resize(queueFamilyCount);

	vkGetPhysicalDeviceQueueFamilyProperties(m_hVkSelectedPhysicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
			
		// Ensure queue supports graphics, has more than one queue, and supports presenting

		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {

			VkBool32 fPresentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_hVkSelectedPhysicalDevice, i, m_hVkSurfaceKHR, &fPresentSupport);

			if (fPresentSupport) {
				m_vulkanQueueFamilyIndices.graphicsFamily = i;
				m_vulkanQueueFamilyIndices.presentFamily = i;
				m_vulkanQueueFamilyIndices.fValid = true;

				m_vulkanQueueFamilyIndices.fVulkanDeviceExtensionsSupported = 
					VulkanQueueFamilyIndices::CheckVulkanDeviceExtensionSupport(
						m_hVkSelectedPhysicalDevice, 
						m_vulkanDeviceRequiredExtensions
					);

				break;
			}
		}
		i++;
	}

Error:
	return r;
}

VulkanSwapChainSupportDetails VulkanSwapChainSupportDetails::QueryVulkanSwapChainSupport(VkPhysicalDevice vkDevice) {
	VulkanSwapChainSupportDetails vulkanSwapChainSupportDetails;

	// 

	return vulkanSwapChainSupportDetails;
}


RESULT VulkanApp::InitializeLogicalDevice() {
	RESULT r = R_PASS;

	CRM(InitializeVulkanDeviceExtensions(), "Failed to initialize vulkan device extensions");

	{

		// Create the queue we will be using
		// Note: We can have multiple queues - this is how we accomplish multi-thread access
		// Idea: Each application could in theory have a queue

		m_vkQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		m_vkQueueCreateInfo.queueFamilyIndex = m_vulkanQueueFamilyIndices.graphicsFamily;
		m_vkQueueCreateInfo.queueCount = 1;

		// Device Features

		m_vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		//m_vkDeviceCreateInfo.pQueueCreateInfos = &m_vkQueueCreateInfo;
		//m_vkDeviceCreateInfo.queueCreateInfoCount = 1;
		m_vkDeviceCreateInfo.pEnabledFeatures = &m_vkPhysicalDeviceFeatures;

		// Device Extensions
		m_vkDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_vulkanDeviceRequiredExtensions.size());
		m_vkDeviceCreateInfo.ppEnabledExtensionNames = m_vulkanDeviceRequiredExtensions.data();

		// This is technically ignored, but good to set anyways
		if (m_fValidationLayersEnabled) {
			m_vkDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_vulkanValidationLayers.size());
			m_vkDeviceCreateInfo.ppEnabledLayerNames = m_vulkanValidationLayers.data();
		}
		else {
			m_vkDeviceCreateInfo.enabledLayerCount = 0;
		}

		// Set up queues
		float queuePriority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			m_vulkanQueueFamilyIndices.graphicsFamily,
			m_vulkanQueueFamilyIndices.presentFamily
		};

		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo vkDeviceQueueCreateInfo = {};

			vkDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			vkDeviceQueueCreateInfo.queueFamilyIndex = queueFamily;
			vkDeviceQueueCreateInfo.queueCount = 1;
			vkDeviceQueueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(vkDeviceQueueCreateInfo);
		}

		m_vkDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		m_vkDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		//m_vkQueueCreateInfo.pQueuePriorities = &queuePriority;

		CBM(vkCreateDevice(m_hVkSelectedPhysicalDevice, &m_vkDeviceCreateInfo, nullptr, &m_hVkLogicalDevice) == VK_SUCCESS,
			"Failed to create vulkan logical device");

		// Set up the queue

		vkGetDeviceQueue(m_hVkLogicalDevice, m_vulkanQueueFamilyIndices.graphicsFamily, 0, &m_hVkGraphicsQueue);
		CNM(m_hVkGraphicsQueue, "Failed to get graphics queue");

		vkGetDeviceQueue(m_hVkLogicalDevice, m_vulkanQueueFamilyIndices.presentFamily, 0, &m_hVkPresentationQueue);
		CNM(m_hVkPresentationQueue, "Failed to get presentation queue");

	}

Error:
	return r;
}

RESULT VulkanApp::MainLoop() {
	RESULT r = R_PASS;	

	while (!glfwWindowShouldClose(m_pglfwWindow)) {
		glfwPollEvents();
	}

Error:
	return r;
}

RESULT VulkanApp::CleanUp() {
	RESULT r = R_PASS;

	// 
	if (m_fValidationLayersEnabled) {
		DestroyDebugUtilsMessengerEXT(m_hVkInstance, m_vulkanDebugMessenger, nullptr);
	}

	// Destroy Vulkan Logical Device
	if (m_hVkLogicalDevice != nullptr) {
		vkDestroyDevice(m_hVkLogicalDevice, nullptr);
		m_hVkLogicalDevice = nullptr;
	}

	if (m_hVkSurfaceKHR != nullptr) {
		vkDestroySurfaceKHR(m_hVkInstance, m_hVkSurfaceKHR, nullptr);
		m_hVkSurfaceKHR = nullptr;
	}

	// Destroy Vulkan Instance
	if (m_hVkInstance != nullptr) {
		vkDestroyInstance(m_hVkInstance, nullptr);
		m_hVkInstance = nullptr;
	}


	// Clean up window
	if (m_pglfwWindow != nullptr) {
		glfwDestroyWindow(m_pglfwWindow);
		m_pglfwWindow = nullptr;
	}

	// Clean up GLFW
	glfwTerminate();

Error:
	return r;
}
