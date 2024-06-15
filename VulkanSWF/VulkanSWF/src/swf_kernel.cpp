#include "swf_kernel.h"
#include <iostream>
#include <exception>


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
		if (configuration->kernelRadius > MAX_RADIUS) {
			throw std::runtime_error("ERROR: Filter kernel radius has to be 0 - 15.");
		}

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

		std::cout << "Creating Box Filter of size " << kernelWidth << "x" << kernelWidth << std::endl << std::endl;

		for (uint16_t i = 0; i < kernelWidth; ++i) {
			for (uint16_t j = 0; j < kernelWidth; ++j) {
				kernel[i + j * kernelWidth] = avg;
			}
		}
		//printKernel();
	}

	void SWFKernel::createGaussianFilter(const float& sigma) {
		std::cout << "Creating Gaussian Filter of size " << kernelWidth << "x" << kernelWidth << std::endl << std::endl;
		double pi = 2 * asin(1.0);

		double sum = 0.0;

		for (uint16_t row = 0; row < kernelWidth; ++row) {
			uint16_t y = abs(row - kernelWidth / 2);
			for (uint16_t col = 0; col < kernelWidth; ++col) {
				uint16_t x = abs(col - kernelWidth / 2);
				double g1 = 1.0 / (2.0 * pi * sigma * sigma);
				double g2 = -0.5 * (x * x + y * y) / (sigma * sigma);

				kernel[col + row * kernelWidth] = g1 * exp(g2);
				sum += kernel[col + row * kernelWidth];
			}
		}

		for (uint16_t row = 0; row < kernelWidth; ++row) {
			for (uint16_t col = 0; col < kernelWidth; ++col) {
				kernel[col + row * kernelWidth] /= sum;
			}
		}
		//printKernel();
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