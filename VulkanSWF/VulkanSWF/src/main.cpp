
#include<iostream>
#include<stdexcept>
#include<cstdlib>

#include "swf_application.h"


int main() {
	
	swf::SWFApplication application{"SWF"};

	swf::SWFKernelConfiguration kernelConf = {};
	kernelConf.kernelRadius = 2;
	kernelConf.kernelType = swf::SWF_KERNEL_TYPE_BOX;

	application.configureKernel(kernelConf);

	try {
		application.execute();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

