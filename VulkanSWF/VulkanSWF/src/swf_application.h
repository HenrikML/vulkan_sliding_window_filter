#pragma once

#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>

#include "swf_kernel.h"

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

		void execute(const char* imagePath, const SWFKernelConfiguration* kernelConf);
	private:
		// Private variables
		const std::string applicationName;

		const uint32_t WORKGROUPS = 32;

		vk::Instance vulkanInstance;
		vk::PhysicalDevice physicalDevice;
		vk::Device logicalDevice;
		uint32_t computeQueueFamilyIndex;

		SWFKernel kernel;

		unsigned char* imageData;
		int imageHeight;
		int imageWidth;
		int imageChannels;

		vk::Buffer inputBuffer;
		vk::Buffer outputBuffer;
		vk::DeviceSize bufferSize;
		uint64_t elements;

		vk::DeviceMemory inputBufferMemory;
		vk::DeviceMemory outputBufferMemory;

		vk::ShaderModule compShaderModule;
		vk::DescriptorSetLayout descriptorSetLayout;

		vk::PipelineLayout pipelineLayout;
		vk::PipelineCache pipelineCache;
		vk::Pipeline pipeline;

		vk::DescriptorPool descriptorPool;
		vk::DescriptorSet descriptorSet;

		vk::CommandPool commandPool;
		vk::CommandBuffer commandBuffer;

		vk::Fence fence;

		// Private methods
		void createVulkanInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();
		
		void createBuffers();
		bool readImage(const char* imagePath);
		void mapDataToMemory();
		void createShaderModule();
		void createDescriptorSetLayout();
		void createPipeline();
		void createDescriptorSet();
		void createCommandBuffer();
		void submitCommands();

		// Helpers
		int pickPhysicalDeviceHelper(const std::vector<vk::PhysicalDevice>& physicalDeviceVec) const;
		void printPhysicalDeviceInfo(const vk::PhysicalDeviceProperties& deviceProps) const;
		void readImage(char* filename);
		std::vector<char> readShader(const std::string& filepath);

	};

}