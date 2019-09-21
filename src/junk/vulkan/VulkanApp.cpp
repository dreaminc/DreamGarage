#include "VulkanApp.h"

#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

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

	CRM(InitWindow(), "Vulkan initialization failed");

	CRM(InitializeVulkan(), "Vulkan initialization failed");

	CRM(MainLoop(), "Main Loop Error");

	CRM(CleanUp(), "Clean up failed");

Error:
	return r;
}

RESULT VulkanApp::InitWindow() {
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

RESULT VulkanApp::CreateVulkanInstance() {
	RESULT r = R_PASS;
	
	VkApplicationInfo appInfo = {};
	VkInstanceCreateInfo createInfo = {};

	// App Info
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Create Info
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Extensions
	CRM(InitializeVulkanExtensions(), "Failed to initialize vulkan extensions");
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_vulkanRequiredExtensions.size());
	createInfo.ppEnabledExtensionNames = m_vulkanRequiredExtensions.data();

	// Enable validation support if our layers are supported
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (m_fValidationLayersEnabled) {
		CRM(CheckValidationLayerSupport(), "Vulkan validation layer check failed");

		createInfo.enabledLayerCount = static_cast<uint32_t>(m_vulkanValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_vulkanValidationLayers.data();

		CRM(PopulateDebugMessengerCreateInfo(debugCreateInfo), 
			"Failed to populate debug messenger info for instance creation");

		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)(&debugCreateInfo);
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// Create our instance
	CBM(vkCreateInstance(&createInfo, nullptr, &m_vkInstance) == VK_SUCCESS, "Failed to create VK instance");

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
	CBM((CreateDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_vulkanDebugMessenger) == VK_SUCCESS),
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

	CRM(InitializePhysicalDevices(), "Failed to initialize physical devices");

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

RESULT VulkanApp::InitializePhysicalDevices() {
	RESULT r = R_PASS;

	vkEnumeratePhysicalDevices(m_vkInstance, &m_vkPhysicalDevices_n, nullptr);
	CBM((m_vkPhysicalDevices_n > 0), "No vulkan physical devices found");

	m_vkPhysicalDeviceHandles = std::vector<VkPhysicalDevice>(m_vkPhysicalDevices_n);
	vkEnumeratePhysicalDevices(m_vkInstance, &m_vkPhysicalDevices_n, m_vkPhysicalDeviceHandles.data());

	for (const auto& vkDeviceHandle : m_vkPhysicalDeviceHandles) {
		if (IsVulkanDeviceSuitable(vkDeviceHandle)) {
			if (m_hVkSelectedPhysicalDevice == nullptr) {
				m_hVkSelectedPhysicalDevice = vkDeviceHandle;

				
			}
		}
	}

	VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(m_hVkSelectedPhysicalDevice, &vkPhysicalDeviceProperties);
	DEBUG_LINEOUT("Selected device: %s", vkPhysicalDeviceProperties.deviceName);

	CNM(m_hVkSelectedPhysicalDevice, "Failed to find suitable vk physical device");


Error:
	return r;
}

RESULT VulkanApp::SelectPhysicalDevice() {
	RESULT r = R_PASS;



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
		DestroyDebugUtilsMessengerEXT(m_vkInstance, m_vulkanDebugMessenger, nullptr);
	}

	// Destroy Vulkan Instance
	vkDestroyInstance(m_vkInstance, nullptr);

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
