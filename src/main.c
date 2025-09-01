#include "includes/main.h"

int main(int argc, char* argv[]) {
	initialize_engine();

	wchar_t* ir = read_file(L"main.ir");

	printf("%S", ir);

	return 0;
}