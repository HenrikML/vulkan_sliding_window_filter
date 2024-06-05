#include "swf_application.h"
#include <iostream>

namespace swf {
	SWFApplication::SWFApplication(std::string applicationName) 
		: applicationName(applicationName) {
		createVulkanInstance();
		createPhysicalDevice();
		createLogicalDevice();
	}

	SWFApplication::~SWFApplication() {
		logicalDevice.destroy();
		vulkanInstance.destroy();
	}

	void SWFApplication::execute() {
	}


	void SWFApplication::createVulkanInstance() {
		vk::ApplicationInfo applicationInfo{
			applicationName.c_str(),	// Application Name
			1,							// Application Version
			nullptr,					// Engine Name
			0,							// Engine Version
			VK_API_VERSION_1_0			// Vulkan API Version
		};

		std::vector<const char*> instanceLayers = {};

		#ifdef DEBUG
			instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
		#endif

		vk::InstanceCreateInfo instanceCreateInfo{
			vk::InstanceCreateFlags(),	// Flags
			&applicationInfo,			// Application Info
			instanceLayers,				// Enabled Layers
			{}							// Enabled extensions
		};

		vulkanInstance = vk::createInstance(instanceCreateInfo);
	}

	void SWFApplication::createPhysicalDevice() {

	}

	void SWFApplication::createLogicalDevice() {

	}
}