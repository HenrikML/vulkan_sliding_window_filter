#include "swf_kernel.h"
#include <iostream>

namespace swf {
	SWFKernel::SWFKernel() {
		kernelWidth = UINT16_MAX;
	}

	SWFKernel::~SWFKernel() {
		delete[] kernel;
	}

	void SWFKernel::setKernel(const SWFKernelConfiguration& configuration) {
		if (kernelWidth != configuration.kernelRadius * 2 + 1) {
			if (kernelWidth < UINT16_MAX) {
				delete[] kernel;
			}
			kernelWidth = configuration.kernelRadius * 2 + 1;
			kernel = new float[kernelWidth * kernelWidth];
		}

		switch (configuration.kernelType) {
		case SWF_KERNEL_TYPE_BOX:
			createBoxFilter(configuration.kernelRadius);
			break;

		case SWF_KERNEL_TYPE_GAUSSIAN:
			createGaussianFilter(configuration.kernelRadius, configuration.sigma);
			break;

		default:
			createBoxFilter(0);
			break;
		}

	}

	void SWFKernel::createBoxFilter(const uint8_t& size) {

		float avg = 1 / float( (kernelWidth) * (kernelWidth) );

		std::cout << "Creating Box Filter of size " << kernelWidth << "x" << kernelWidth << ":" << std::endl;

		for (uint16_t i = 0; i < kernelWidth; ++i) {
			for (uint16_t j = 0; j < kernelWidth; ++j) {
				kernel[i + j * kernelWidth] = avg;
			}
		}
		printKernel();
	}

	void SWFKernel::createGaussianFilter(const uint8_t& size, const float& sigma) {
		// TODO: Create gaussian filter
		createBoxFilter(size); // Remove this once gaussian filter is implemented
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