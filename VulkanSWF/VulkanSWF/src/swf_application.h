#pragma once

#include <vulkan/vulkan.hpp>
#include <string>

#include "swf_kernel.h"
#include "stb_image.h"

#ifndef DEBUG
#define DEBUG
#endif

namespace swf {

	// TODO: SWFKernel

	class SWFApplication {
	public:
		// Public variables

		// Public methods
		SWFApplication(std::string applicationName);
		~SWFApplication();

		void execute(const char* imagePath, const SWFKernelConfiguration* kernelConf);
	private:
		// Private variables
		const std::string applicationName;

		vk::Instance vulkanInstance;
		vk::PhysicalDevice physicalDevice;
		vk::Device logicalDevice;
		uint32_t computeQueueFamilyIndex;

		SWFKernel kernel;

		// Private methods
		void createVulkanInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();

		// Helpers
		int pickPhysicalDeviceHelper(const std::vector<vk::PhysicalDevice>& physicalDeviceVec) const;
		void printPhysicalDeviceInfo(const vk::PhysicalDeviceProperties& deviceProps) const;

	};

}