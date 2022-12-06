#include <iostream>
#include <string>
#include <time.h>
using namespace std;

int main() {
	int result = 0;
	clock_t startTime, stopTime;
	const long int repeating_time = 500000000;
	startTime = clock();

	for (long int i=0; i<repeating_time; i++) {
		if(result > 1000)
			result -= 1;
		else
			result += 1;
	}

	stopTime = clock();
	std::cout << "Execution Time: " << double(stopTime - startTime)/CLOCKS_PER_SEC << " seconds" << std::endl;
	
	return 0;
}
