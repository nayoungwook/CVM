#include "error.h"

void Error::throw_error(const std::string& message, int line_number) {
	std::cout << "\n[Chestnut Error] < Error has been occurred at line number \"" << std::to_string(line_number) << "\" > \n";
	std::cout << std::endl << message << std::endl;
	std::cout << "\nError Code : ";
	std::cout << "0x" << std::hex << (int)error_code << std::endl;

	exit(1);
}