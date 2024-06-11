#include "swf_application.h"
#include <iostream>
#include <exception>
#include <filesystem>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace swf {
	SWFApplication::SWFApplication(std::string applicationName) 
		: applicationName(applicationName) {
		createVulkanInstance();
		pickPhysicalDevice();
		createLogicalDevice();
	}

	SWFApplication::~SWFApplication() {
		logicalDevice.resetCommandPool(commandPool, vk::CommandPoolResetFlags());
		logicalDevice.destroyFence(fence);
		logicalDevice.destroyCommandPool(commandPool);
		logicalDevice.destroyDescriptorPool(descriptorPool);
		logicalDevice.destroyPipeline(pipeline);
		logicalDevice.destroyPipelineCache(pipelineCache);
		logicalDevice.destroyPipelineLayout(pipelineLayout);
		logicalDevice.destroyDescriptorSetLayout(descriptorSetLayout);
		logicalDevice.destroyShaderModule(compShaderModule);
		logicalDevice.freeMemory(outputBufferMemory);
		logicalDevice.freeMemory(inputBufferMemory);
		logicalDevice.destroyBuffer(outputBuffer);
		logicalDevice.destroyBuffer(inputBuffer);
		logicalDevice.destroy();
		vulkanInstance.destroy();
		stbi_image_free(imageData);
	}
	
	
	void SWFApplication::execute(const char* imagePath, const SWFKernelConfiguration* kernelConf) {

		if ( !readImage(imagePath) ) {
			throw std::runtime_error("ERROR: Could not load image");
		}

		createBuffers();
		mapDataToMemory();
		createShaderModule();
		createDescriptorSetLayout();
		createPipeline();
		createDescriptorSet();
		createCommandBuffer();
		submitCommands();
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

	void SWFApplication::pickPhysicalDevice() {
		std::cout << "Searching for physical devices..." << std::endl << std::endl;
		std::vector<vk::PhysicalDevice> physicalDeviceVec = vulkanInstance.enumeratePhysicalDevices();
		if (physicalDeviceVec.size() < 1) {
			throw std::runtime_error("ERROR: 0 physical devices detected!");
		}

		int deviceIndex = pickPhysicalDeviceHelper(physicalDeviceVec);

		physicalDevice = physicalDeviceVec[deviceIndex];
		std::cout << "Picked physical device:" << std::endl;
		printPhysicalDeviceInfo(physicalDevice.getProperties());
	}

	void SWFApplication::createLogicalDevice() {
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

		computeQueueFamilyIndex = 0;
		for (const vk::QueueFamilyProperties& prop : queueFamilyProperties) {
			if (prop.queueFlags & vk::QueueFlagBits::eCompute) {
				break;
			}
			++computeQueueFamilyIndex;
		}

		float queuePriority = 1.0f;
		vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
			vk::DeviceQueueCreateFlags(),	// Flags
			computeQueueFamilyIndex,		// Queue Family Index
			1,								// Queue Count
			&queuePriority,					// Queue Priority
		};

		vk::DeviceCreateInfo deviceCreateInfo{
			vk::DeviceCreateFlags(),		// Flags
			deviceQueueCreateInfo			// Device Queue Create Info
		};

		logicalDevice = physicalDevice.createDevice(deviceCreateInfo);
	}

	bool SWFApplication::readImage(const char* imagePath) {

		imageData = stbi_load(imagePath, &imageWidth, &imageHeight, &imageChannels, 0);

		if (!imageData) {
			return false;
		}

		std::cout << "Image height: " << imageHeight << std::endl;
		std::cout << "Image width: " << imageWidth << std::endl;
		std::cout << "Color channels: " << imageChannels << std::endl << std::endl;

		elements = imageHeight * imageWidth * imageChannels;
		bufferSize = elements * sizeof(uint32_t);

		return true;
	}

	void SWFApplication::createBuffers() {

		vk::BufferCreateInfo bufferCreateInfo{
			vk::BufferCreateFlags(),
			bufferSize,
			vk::BufferUsageFlagBits::eStorageBuffer,
			vk::SharingMode::eExclusive,
			1,
			&computeQueueFamilyIndex
		};


		inputBuffer = logicalDevice.createBuffer(bufferCreateInfo);
		outputBuffer = logicalDevice.createBuffer(bufferCreateInfo);
	}

	void SWFApplication::mapDataToMemory() {
		vk::MemoryRequirements inputBufferMemoryReq = logicalDevice.getBufferMemoryRequirements(inputBuffer);
		vk::MemoryRequirements outputBufferMemoryReq = logicalDevice.getBufferMemoryRequirements(outputBuffer);

		vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();
		uint32_t memoryTypeIndex = UINT32_MAX;
		vk::DeviceSize memoryHeapSize = UINT64_MAX;

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
			vk::MemoryType memoryType = memoryProperties.memoryTypes[i];
			if ((memoryType.propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) &&
				(memoryType.propertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
				memoryHeapSize = memoryProperties.memoryHeaps[memoryType.heapIndex].size;
				memoryTypeIndex = i;
				break;
			}
		}

		vk::MemoryAllocateInfo inputBufferMemoryAllocInfo(inputBufferMemoryReq.size, memoryTypeIndex);
		vk::MemoryAllocateInfo outputBufferMemoryAllocInfo(outputBufferMemoryReq.size, memoryTypeIndex);

		inputBufferMemory = logicalDevice.allocateMemory(inputBufferMemoryAllocInfo);
		outputBufferMemory = logicalDevice.allocateMemory(outputBufferMemoryAllocInfo);

		uint32_t* data = static_cast<uint32_t*>(logicalDevice.mapMemory(inputBufferMemory, 0, bufferSize));

		for (uint64_t i = 0; i < elements; ++i) {
			data[i] = uint32_t(imageData[i]);
		}
		logicalDevice.unmapMemory(inputBufferMemory);

		logicalDevice.bindBufferMemory(inputBuffer, inputBufferMemory, 0);
		logicalDevice.bindBufferMemory(outputBuffer, outputBufferMemory, 0);
	}

	void SWFApplication::createShaderModule() {
		std::vector<char> compShader = readShader("shaders/filter.comp.spv");

		vk::ShaderModuleCreateInfo shaderModuleCreateInfo{
			vk::ShaderModuleCreateFlags(),							// Flags
			compShader.size(),										// Shader size
			reinterpret_cast<const uint32_t*>(compShader.data()) 	// Shader code
		};

		compShaderModule = logicalDevice.createShaderModule(shaderModuleCreateInfo);
	}


	void SWFApplication::createDescriptorSetLayout() {
		std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings;

		for (uint32_t i = 0; i < 2; i++) {
			vk::DescriptorSetLayoutBinding binding{
				i,
				vk::DescriptorType::eStorageBuffer,
				1,
				vk::ShaderStageFlagBits::eCompute
			};

			descriptorSetLayoutBindings.push_back(binding);
		}

		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{
			vk::DescriptorSetLayoutCreateFlags(),
			descriptorSetLayoutBindings
		};

		descriptorSetLayout = logicalDevice.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
	}

	void SWFApplication::createPipeline() {
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{
			vk::PipelineLayoutCreateFlags(),
			descriptorSetLayout
		};

		vk::PipelineCacheCreateInfo pipelineCacheCreateInfo{};

		pipelineLayout = logicalDevice.createPipelineLayout(pipelineLayoutCreateInfo);
		pipelineCache = logicalDevice.createPipelineCache(pipelineCacheCreateInfo);

		vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo{
			vk::PipelineShaderStageCreateFlags(),	// Flags
			vk::ShaderStageFlagBits::eCompute,		// Shader Stage
			compShaderModule,						// Shader Module
			"main"									// Main Function
		};

		vk::ComputePipelineCreateInfo computePipelineCreateInfo{
			vk::PipelineCreateFlags(),				// Flags
			pipelineShaderStageCreateInfo,			// Shader Stage Create Info
			pipelineLayout							// Pipeline Layout
		};

		vk::Result result;
		std::tie( result, pipeline ) = logicalDevice.createComputePipeline(pipelineCache, computePipelineCreateInfo);

		switch (result)
		{
		case vk::Result::eSuccess:
			break;

		default:
			throw std::runtime_error("ERROR: Failed to create pipeline");

		}
	}


	void SWFApplication::createDescriptorSet() {
		vk::DescriptorPoolSize descriptorPoolSize{
			vk::DescriptorType::eStorageBuffer,		// Descriptor Type
			2										// Descriptor Count
		};
		vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{
			vk::DescriptorPoolCreateFlags(),		// Flags
			1,										// Pool Size Count
			descriptorPoolSize						// Pool Sizes
		};
		descriptorPool = logicalDevice.createDescriptorPool(descriptorPoolCreateInfo);

		vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo{
			descriptorPool,							// Descriptor Pool
			1,										// Descriptor Set Count
			&descriptorSetLayout					// Layouts
		};

		const std::vector<vk::DescriptorSet> descriptorSets = logicalDevice.allocateDescriptorSets(descriptorSetAllocateInfo);
		descriptorSet = descriptorSets.front();
		vk::DescriptorBufferInfo inputBufferInfo{
			inputBuffer,
			0,
			bufferSize
		};
		vk::DescriptorBufferInfo outputBufferInfo{
			outputBuffer,
			0,
			bufferSize
		};

		std::vector<vk::WriteDescriptorSet> writeDescriptorSets;

		vk::WriteDescriptorSet inputWriteDescriptorSet{
			descriptorSet,
			0,
			0,
			1,
			vk::DescriptorType::eStorageBuffer,
			nullptr,
			&inputBufferInfo
		};

		vk::WriteDescriptorSet outputWriteDescriptorSet{
			descriptorSet,
			1,
			0,
			1,
			vk::DescriptorType::eStorageBuffer,
			nullptr,
			&outputBufferInfo
		};

		writeDescriptorSets.push_back(inputWriteDescriptorSet);
		writeDescriptorSets.push_back(outputWriteDescriptorSet);

		logicalDevice.updateDescriptorSets(writeDescriptorSets, {});
	}


	void SWFApplication::createCommandBuffer() {
		vk::CommandPoolCreateInfo commandPoolCreateInfo{
			vk::CommandPoolCreateFlags(),
			computeQueueFamilyIndex
		};

		commandPool = logicalDevice.createCommandPool(commandPoolCreateInfo);

		vk::CommandBufferAllocateInfo commandBufferAllocateInfo{
			commandPool,						// Command Pool
			vk::CommandBufferLevel::ePrimary,	// Command Buffer Level
			1									// Command Buffer Count
		};
		const std::vector<vk::CommandBuffer> commandBuffers = logicalDevice.allocateCommandBuffers(commandBufferAllocateInfo);
		commandBuffer = commandBuffers.front();
	}

	void SWFApplication::submitCommands() {
		vk::CommandBufferBeginInfo commandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
		commandBuffer.begin(commandBufferBeginInfo);
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, { descriptorSet }, {});
		commandBuffer.dispatch(elements / WORKGROUPS, 1, 1);
		commandBuffer.end();

		vk::Queue queue = logicalDevice.getQueue(computeQueueFamilyIndex, 0);
		fence = logicalDevice.createFence(vk::FenceCreateInfo());

		vk::SubmitInfo submitInfo{
			0,
			nullptr,
			nullptr,
			1,
			&commandBuffer
		};

		queue.submit({ submitInfo }, fence);
		logicalDevice.waitForFences({ fence }, true, UINT64_MAX);


		uint8_t* imageOutput = new uint8_t[elements];
		uint32_t* data = static_cast<uint32_t*>(logicalDevice.mapMemory(outputBufferMemory, 0, bufferSize));

		for (uint64_t i = 0; i < elements; ++i) {
			imageOutput[i] = uint8_t(data[i]);
		}

		logicalDevice.unmapMemory(outputBufferMemory);

		stbi_write_jpg("img\\output.jpg", imageWidth, imageHeight, imageChannels, imageOutput, 100);

		delete[] imageOutput;
	}

	// -------- Helper Functions ----------

	std::vector<char> SWFApplication::readShader(const std::string& filepath) {
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	int SWFApplication::pickPhysicalDeviceHelper(const std::vector<vk::PhysicalDevice>& physicalDeviceVec) const {
		int deviceIndex = 0;
		if (physicalDeviceVec.size() > 1) {
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
		return deviceIndex;
	}

	void SWFApplication::printPhysicalDeviceInfo(const vk::PhysicalDeviceProperties& deviceProps) const {
		std::cout << "    Device Name: " << deviceProps.deviceName << std::endl;
		std::cout << "    Device ID: " << deviceProps.deviceID << std::endl;
		std::cout << "    Vulkan API Version:" << VK_VERSION_MAJOR(deviceProps.apiVersion) << "."
			<< VK_VERSION_MINOR(deviceProps.apiVersion) << "."
			<< VK_VERSION_PATCH(deviceProps.apiVersion) << std::endl << std::endl;
	}
}