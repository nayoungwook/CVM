#include "includes/main.h"

int main(int argc, char* argv[]) {
	initialize_engine();

	wchar_t* ir = read_file(L"main.ir");

	parse_code_data(ir);
	
	return 0;
}