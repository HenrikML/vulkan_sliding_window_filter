#include "swf_kernel.h"
#include <iostream>

namespace swf {
	SWFKernel::SWFKernel() {
		kernelWidth = 1;
		kernel = new float[kernelWidth];
		createBoxFilter();
	}

	SWFKernel::~SWFKernel() {
		delete[] kernel;
	}

	void SWFKernel::setKernel(const SWFKernelConfiguration* configuration) {
		uint16_t newWidth = configuration->kernelRadius * 2 + 1;
		if (kernelWidth != newWidth) {
			delete[] kernel;
			kernelWidth = newWidth;
			kernel = new float[kernelWidth * kernelWidth];
		}

		switch (configuration->kernelType) {
		case SWFKernelType::SWF_KERNEL_TYPE_BOX:
			createBoxFilter();
			break;

		case SWFKernelType::SWF_KERNEL_TYPE_GAUSSIAN:
			createGaussianFilter(configuration->sigma);
			break;

		default:
			createBoxFilter();
			break;
		}
	}

	void SWFKernel::createBoxFilter() {

		float avg = 1 / float( (kernelWidth) * (kernelWidth) );

		std::cout << "Creating Box Filter of size " << kernelWidth << "x" << kernelWidth << std::endl;

		for (uint16_t i = 0; i < kernelWidth; ++i) {
			for (uint16_t j = 0; j < kernelWidth; ++j) {
				kernel[i + j * kernelWidth] = avg;
			}
		}
		//printKernel();
	}

	void SWFKernel::createGaussianFilter(const float& sigma) {
		// TODO: Create gaussian filter
		createBoxFilter(); // Remove this once gaussian filter is implemented
	}

	void SWFKernel::printKernel() {

		for (uint16_t i = 0; i < kernelWidth; ++i) {
			for (uint16_t j = 0; j < kernelWidth; ++j) {
				std::cout << kernel[i + j * kernelWidth] << " ";
			}
			std::cout << std::endl;
		}
	}
}