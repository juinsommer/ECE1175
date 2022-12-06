#include <iostream>
#include <fstream>
#include <string>
#include "functions.h"

int main(int argc, char** argv) {
	std::string governor = argv[1];
	setGovernor(governor);

	return 0;
}
