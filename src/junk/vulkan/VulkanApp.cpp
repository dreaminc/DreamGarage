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

	CRM(InitVulkan(), "Vulkan initialization failed");

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

	for(int i = 0; i < glfwExtensionCount; i++) {
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
	createInfo.enabledExtensionCount = m_vulkanRequiredExtensions.size();
	createInfo.ppEnabledExtensionNames = m_vulkanRequiredExtensions.data();

	// Enable validation support if our layers are supported
	if (m_fValidationLayersEnabled) {
		CRM(CheckValidationLayerSupport(), "Vulkan validation layer check failed");

		createInfo.enabledLayerCount = static_cast<uint32_t>(m_vulkanValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_vulkanValidationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	// Create our instance
	CBM(vkCreateInstance(&createInfo, nullptr, &m_vkInstance) == VK_SUCCESS, "Failed to create VK instance");

Error:
	return r;
}

RESULT VulkanApp::InitVulkan() {
	RESULT r = R_PASS;

	// Disable validation layers for debug builds
#ifndef _DEBUG
	bool m_fValidationLayersEnabled = false;
#endif

	CRM(CreateVulkanInstance(), "Failed to create vulkan instance");

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
