#pragma once

#include<vector>

namespace swf {
	
	enum SWFKernelType {
		SWF_KERNEL_TYPE_BOX = 0,
		SWF_KERNEL_TYPE_GAUSSIAN = 1
	};

	struct SWFKernelConfiguration {
		uint8_t kernelRadius;
		SWFKernelType kernelType;
		float sigma;				// Gaussian filter only
	};
	
	class SWFKernel {
	public:
		SWFKernel();
		~SWFKernel();
		
		float* getKernel() const { return kernel; };
		uint16_t getKernelSize() const { return kernelWidth; };

		void setKernel(const SWFKernelConfiguration& configuration);
		
	private:
		// Private Variables
		float* kernel;		// 2D kernel represented as 1D array
		uint16_t kernelWidth;

		// Private Methods
		void createBoxFilter(const uint8_t& size);
		void createGaussianFilter(const uint8_t& size, const float& sigma);
		void printKernel();

	};
}