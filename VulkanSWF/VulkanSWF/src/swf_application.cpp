#include "swf_application.h"
#include <iostream>
#include <exception>

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
			{}							// Enabled Extensions
		};

		vulkanInstance = vk::createInstance(instanceCreateInfo);
	}

	void SWFApplication::createPhysicalDevice() {
		std::cout << "Searching for physical devices..." << std::endl << std::endl;
		std::vector<vk::PhysicalDevice> physicalDeviceVec = vulkanInstance.enumeratePhysicalDevices();

		int deviceIndex = 0;
		if (physicalDeviceVec.size() > 1
			) {
			std::cout << "Found multiple devices" << std::endl;
			std::cout << "Pick a device (0 - " << physicalDeviceVec.size() - 1 << "): " << std::endl;
			for (int i = 0; i < physicalDeviceVec.size(); ++i) {
				vk::PhysicalDeviceProperties deviceProperties = physicalDeviceVec[i].getProperties();
				std::cout << "    " << i << " - " << deviceProperties.deviceName << std::endl;
			}
			std::cout << ">> ";
			std::cin >> deviceIndex;
			while (!std::cin.good() || deviceIndex < 0 || deviceIndex >= physicalDeviceVec.size()) {
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Pick a valid device (0 - " << physicalDeviceVec.size() - 1 << "): " << std::endl;
				for (int i = 0; i < physicalDeviceVec.size(); ++i) {
					vk::PhysicalDeviceProperties deviceProperties = physicalDeviceVec[i].getProperties();
					std::cout << "    " << i << " - " << deviceProperties.deviceName << std::endl;
				}
				std::cout << ">> ";
				std::cin >> deviceIndex;
			}
			std::cout << std::endl;
		}
		else if (physicalDeviceVec.size() < 1) {
			throw std::runtime_error("ERROR: 0 physical devices detected!");
		}

		physicalDevice = physicalDeviceVec[deviceIndex];
		std::cout << "Picked physical device:" << std::endl;
		printPhysicalDeviceInfo(physicalDevice.getProperties());
	}

	void SWFApplication::createLogicalDevice() {

	}

	// -------- Helper Functions ----------

	void SWFApplication::printPhysicalDeviceInfo(vk::PhysicalDeviceProperties& deviceProps) const {
		std::cout << "    Device Name: " << deviceProps.deviceName << std::endl;
		std::cout << "    Device ID: " << deviceProps.deviceID << std::endl;
		std::cout << "    Vulkan API Version:" << VK_VERSION_MAJOR(deviceProps.apiVersion) << "."
			<< VK_VERSION_MINOR(deviceProps.apiVersion) << "."
			<< VK_VERSION_PATCH(deviceProps.apiVersion) << std::endl;
	}
}