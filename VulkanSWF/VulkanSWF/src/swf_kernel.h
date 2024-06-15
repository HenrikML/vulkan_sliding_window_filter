#pragma once

#include<vector>

#define _USE_MATH_DEFINES
#include <cmath>

namespace swf {
	
	enum class SWFKernelType {
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
		uint16_t getKernelWidth() const { return kernelWidth; };

		void setKernel(const SWFKernelConfiguration* configuration);
		
	private:
		// Private Variables
		float* kernel;		// 2D kernel represented as 1D array
		uint16_t kernelWidth;

		const uint8_t MAX_RADIUS = 15;

		// Private Methods
		void createBoxFilter();
		void createGaussianFilter(const float& sigma);
		void printKernel();

	};
}