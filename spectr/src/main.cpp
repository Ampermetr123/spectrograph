#include <iostream>
#include <string>
#include "controller.h"
#include "capture.h"


int main(int argc, char** argv) {

	// Config file ?
	std::string config_file, default_config_file;
	auto pos = std::string(argv[0]).find_last_of('.');
	if (pos != std::string::npos) {
		//std::cout << std::string(argv[0]) << " " << pos << "\n";
		default_config_file = std::string(argv[0]).substr(0, pos) + ".yml";
	}
	else {
		default_config_file = std::string(argv[0]) + ".yml";
	}
	if (argc > 1) {
		config_file = std::string(argv[1]);
		if (config_file == "--help" || config_file == "-h") {
			std::cout << "Usage " << argv[0] << " [yaml_config_file]\n";
			std::cout << "Default config file is " << default_config_file << std::endl;
			return 0;
		}
	}
	else {
		config_file = default_config_file;
	}


	try {
		return Controller::instance(config_file).run();
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what();
		return 0;
	}
}
