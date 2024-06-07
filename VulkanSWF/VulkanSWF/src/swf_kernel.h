#pragma once

#include<vector>

namespace swf {
	
	enum SWFKernelType {
		SWF_KERNEL_TYPE_BOX = 0,
		SWF_KERNEL_TYPE_GAUSSIAN = 1
	};

	struct SWFKernelConfiguration {
		uint8_t kernelSize;			// Radius
		SWFKernelType kernelType;
		float sigma;				// Gaussian filter only
	};

	typedef std::vector<std::vector<float>> SWFKernelData;
	
	class SWFKernel {
	public:
		SWFKernel();
		~SWFKernel();
		
		SWFKernelData getKernel() const { return kernel; };
		uint8_t getKernelSize() const { return kernelSize; };

		void setKernel(const SWFKernelConfiguration& configuration);
		
	private:
		// Private Variables
		SWFKernelData kernel;
		uint8_t kernelSize;

		// Private Methods
		void createBoxFilter(const uint8_t& size);
		void createGaussianFilter(const uint8_t& size, const float& sigma);
		void printKernel();

	};
}