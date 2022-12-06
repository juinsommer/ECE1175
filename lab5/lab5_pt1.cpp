#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include "functions.h"

int main(int argc, char** argv) {
	int mode = std::stoi(argv[1]);

	if(mode == 0) {
		vgetFrequency();
		runWorkload();	
	}

	else if(mode == 1) {
		clock_t startTime, stopTime;
		startTime = clock();
		runWorkload();
		stopTime = clock();
		std::cout << "\nExecution Time: " << double(stopTime - startTime)/CLOCKS_PER_SEC << " seconds" << std::endl;
	}

	else
		std::cout << "\nError Lab5_pt1.cpp: No mode selected." << std::endl;

	return 0;
}

