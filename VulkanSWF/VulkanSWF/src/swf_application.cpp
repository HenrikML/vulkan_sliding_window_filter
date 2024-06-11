#include "swf_application.h"
#include <iostream>
#include <exception>
#include <filesystem>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace swf {
	SWFApplication::SWFApplication(std::string applicationName) 
		: applicationName(applicationName) {
		createVulkanInstance();
		pickPhysicalDevice();
		createLogicalDevice();
	}

	SWFApplication::~SWFApplication() {
		logicalDevice.destroyShaderModule(compShaderModule);
		logicalDevice.freeMemory(outputBufferMemory);
		logicalDevice.freeMemory(inputBufferMemory);
		logicalDevice.destroyBuffer(outputBuffer);
		logicalDevice.destroyBuffer(inputBuffer);
		logicalDevice.destroy();
		vulkanInstance.destroy();
	}
	
	
	void SWFApplication::execute(const char* imagePath, const SWFKernelConfiguration* kernelConf) {

		if ( !readImage(imagePath) ) {
			throw std::runtime_error("ERROR: Could not load image");
		}

		createBuffers();
		mapDataToMemory();
		createShaderModule();
		createDescriptorSetLayout();
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

		bufferSize = imageHeight * imageWidth * imageChannels * sizeof(uint8_t);

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

		unsigned char* data = static_cast<unsigned char*>(logicalDevice.mapMemory(inputBufferMemory, 0, bufferSize));
		data = imageData;
		logicalDevice.unmapMemory(inputBufferMemory);
		stbi_image_free(imageData);

		logicalDevice.bindBufferMemory(inputBuffer, inputBufferMemory, 0);
		logicalDevice.bindBufferMemory(outputBuffer, outputBufferMemory, 0);
	}

	void SWFApplication::createShaderModule() {
		std::vector<char> compShader = readShader("shaders/filter.comp.spv");

		vk::ShaderModuleCreateInfo shaderModuleCreateInfo(
			vk::ShaderModuleCreateFlags(),							// Flags
			compShader.size(),										// Shader size
			reinterpret_cast<const uint32_t*>(compShader.data()));	// Shader code

		compShaderModule = logicalDevice.createShaderModule(shaderModuleCreateInfo);
	}


	void SWFApplication::createDescriptorSetLayout() {

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