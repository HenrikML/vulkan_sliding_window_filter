
#include<iostream>
#include<stdexcept>
#include<cstdlib>

#include "swf_application.h"


int main() {
	
	swf::SWFApplication application{"SWF"};

	swf::SWFKernelConfiguration kernelConf = {};
	kernelConf.kernelRadius = 5; // Max 15
	kernelConf.kernelType = swf::SWFKernelType::SWF_KERNEL_TYPE_GAUSSIAN;
	kernelConf.sigma = 4.0f;

	try {
		application.execute("img\\input.jpg", &kernelConf, "img\\output.jpg");
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

