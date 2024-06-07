#include "swf_kernel.h"
#include <iostream>

namespace swf {
	SWFKernel::SWFKernel() {
	}

	SWFKernel::~SWFKernel() {
	}

	void SWFKernel::setKernel(const SWFKernelConfiguration& configuration) {
		kernelSize = configuration.kernelSize;

		switch (configuration.kernelType) {
		case SWF_KERNEL_TYPE_BOX:
			createBoxFilter(configuration.kernelSize);
			break;

		case SWF_KERNEL_TYPE_GAUSSIAN:
			createGaussianFilter(configuration.kernelSize, configuration.sigma);
			break;

		default:
			createBoxFilter(0);
			break;
		}

	}

	void SWFKernel::createBoxFilter(const uint8_t& size) {
		uint16_t row_len = 2 * size + 1;
		float avg = 1 / float( ((row_len) * (row_len)) );

		std::cout << "Creating Box Filter of size " << row_len << "*" << row_len << ":" << std::endl;

		kernel = std::vector<std::vector<float>>(row_len, std::vector<float>(row_len, avg));
		printKernel();
	}

	void SWFKernel::createGaussianFilter(const uint8_t& size, const float& sigma) {
		// TODO: Create gaussian filter
		createBoxFilter(size); // Remove this once gaussian filter is implemented
	}

	void SWFKernel::printKernel() {
		for (auto row : kernel) {
			std::cout << "    ";
			for (auto val : row) {
				std::cout << val << " ";
			}
			std::cout << std::endl;
		}
	}
}