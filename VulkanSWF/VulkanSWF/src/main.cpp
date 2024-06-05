
#include<iostream>
#include<stdexcept>
#include<cstdlib>

#include "swf_application.h"

int main() {
	
	swf::SWFApplication application{"SWF"};
	
	try {
		application.execute();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

