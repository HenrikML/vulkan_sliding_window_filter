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

		vk::Device logicalDevice;
		vk::PhysicalDevice physicalDevice;
		vk::Instance vulkanInstance;

		// Private methods
		void createVulkanInstance();
		void createPhysicalDevice();
		void createLogicalDevice();

	};

}