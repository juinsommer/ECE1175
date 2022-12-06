#include <iostream>
#include <fstream>
#include "functions.h"

int main(int argc, char** argv) {
	int frequency = std::stoi(argv[1]);
 	setFrequency(frequency);

	return 0;
}
