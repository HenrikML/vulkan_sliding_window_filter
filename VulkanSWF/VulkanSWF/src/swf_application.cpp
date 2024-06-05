#include "swf_application.h"

namespace swf {
	SWFApplication::SWFApplication(std::string applicationName) 
		: applicationName(applicationName) {
		createVulkanInstance();
		createPhysicalDevice();
		createLogicalDevice();
	}

	SWFApplication::~SWFApplication() {
	}

	void SWFApplication::execute() {
	}


	void SWFApplication::createVulkanInstance() {
		
	}

	void SWFApplication::createPhysicalDevice() {

	}

	void SWFApplication::createLogicalDevice() {

	}
}