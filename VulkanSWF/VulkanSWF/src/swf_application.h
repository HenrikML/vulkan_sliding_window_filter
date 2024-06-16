#pragma once

#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>

#include "swf_kernel.h"

#ifndef DEBUG
#define DEBUG
#endif

namespace swf {


	struct SWFImageInfo {
		uint32_t width;
		uint32_t height;
		uint32_t channels;
	};

	struct SWFBufferInfo {
		uint32_t binding;
		vk::DescriptorType type;
	};

	struct SWFKernelInfo {
		uint32_t width;
		float* data;
	};

	class SWFApplication {
	public:
		// Public variables

		// Public methods
		SWFApplication(std::string applicationName);
		~SWFApplication();

		void execute(const char* imagePath, const SWFKernelConfiguration* kernelConf, const char* outputPath);
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
		
		SWFImageInfo imageInfo;
		SWFKernelInfo kernelInfo;

		std::vector<SWFBufferInfo> bufferInfos;

		vk::Buffer inputBuffer;
		vk::Buffer outputBuffer;
		vk::DeviceSize ioBufferSize;

		vk::Buffer kernelInfoBuffer;
		vk::DeviceSize kernelInfoWidthBufferSize;
		vk::DeviceSize kernelInfoDataBufferSize;

		vk::Buffer imageInfoBuffer;
		vk::DeviceSize imageInfoBufferSize;

		uint64_t elements;

		vk::DeviceMemory inputBufferMemory;
		vk::DeviceMemory outputBufferMemory;
		vk::DeviceMemory kernelInfoBufferMemory;
		vk::DeviceMemory imageInfoBufferMemory;

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
		void createKernel(const SWFKernelConfiguration* kernelConf);
		bool readImage(const char* imagePath);
		void mapDataToMemory();
		void createShaderModule();
		void createDescriptorSetLayout();
		void createPipeline();
		void createDescriptorSet();
		void createCommandBuffer();
		void submitCommands(const char* outputPath);

		void cleanUp();

		// Helpers
		int pickPhysicalDeviceHelper(const std::vector<vk::PhysicalDevice>& physicalDeviceVec) const;
		void printPhysicalDeviceInfo(const vk::PhysicalDeviceProperties& deviceProps) const;
		void readImage(char* filename);
		std::vector<char> readShader(const std::string& filepath);

	};

}