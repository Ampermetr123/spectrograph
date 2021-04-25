/**
 * @file main.cpp
 * @author Sergey Simonov (sb.simonov@gmail.com)
 * @brief Программа для вычисления и визуализации спектра видеопотока
 * @copyright Copyright (c) 2021
 */

#include <iostream>
#include <string>
#include "controller.h"


int main(int argc, char** argv) {

	// Определение файла с глобальными параметрами программы
	std::string config_file, default_config_file;
	auto pos = std::string(argv[0]).find_last_of('.');
	if (pos != std::string::npos) {
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
		Controller ctrl(config_file);
		return ctrl.run();
	}
	catch (const cv::Exception& ex) {
		std::cerr << "\nRuntime error:\n"<< ex.err << "\n";
		return 0;
	}
	catch (const std::exception& ex) {
		std::cerr << "\nRuntime error:\n" <<ex.what();
		return 0;
	}
}
