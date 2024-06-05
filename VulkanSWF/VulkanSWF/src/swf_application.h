#pragma once

#include <vulkan/vulkan.hpp>
#include <string>

#ifndef DEBUG
#define DEBUG
#endif

namespace swf {

	class SWFApplication {
	public:
		// Public variables

		// Public methods
		SWFApplication(std::string applicationName);
		~SWFApplication();
		void execute();
	private:
		// Private variables
		const std::string applicationName;

		vk::Instance vulkanInstance;
		vk::PhysicalDevice physicalDevice;
		vk::Device logicalDevice;
		uint32_t computeQueueFamilyIndex;

		// Private methods
		void createVulkanInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();

		// Helpers
		int pickPhysicalDevice(const std::vector<vk::PhysicalDevice>& physicalDeviceVec) const;
		void printPhysicalDeviceInfo(const vk::PhysicalDeviceProperties& deviceProps) const;

	};

}