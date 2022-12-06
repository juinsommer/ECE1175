#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include "functions.h"

using namespace std;

const int MIN_FREQ =  600000;
const int MAX_FREQ = 1800000;
const int SCALING_FACTOR =  100000;

int main() {
	setGovernor("userspace");

	long double currUtilization[5] = {0,0,0,0};
	long int currTimeUsage[5] = {0,0,0,0,0};
	long int currentFreq = getFrequency();
	long int currTimeTotal[5] = {0,0,0,0,0};

	while(true) {
		long double  maxUtilization = 0;
		long double desiredFreq = 0;
		long int nextFreq = getFrequency();
		long int nextTimeTotal[5] = {0,0,0,0,0};
		long int nextTimeUsage[5] = {0,0,0,0,0};
		long int nextTimeIdle[5]  = {0,0,0,0,0};	
		long double timeTotal[5] = {0,0,0,0,0};
		long double timeUsage[5] = {0,0,0,0,0};
		long double nextUtilization[5] = {0,0,0,0,0};
		
		
		// iterate for each of the four CPUs
		for(int c = 0; c < 5; c++){	
			string firstLine = readProcStat(c);
			std::string delimiter = " "; 
			std::string wait = "";

			if(c == 0)
				firstLine.erase(0, firstLine.find(delimiter) + delimiter.length() + 1);
			
			else
				firstLine.erase(0, firstLine.find(delimiter) + delimiter.length());
			
			// 8 lines of data = total time since boot
			for(int i = 0; i < 8; i++) {
				wait = firstLine.substr(0, firstLine.find(delimiter));
				
				nextTimeTotal[c] += std::stoi(wait);
				
				// idle times in third and fourth columns	
				if(i == 3 || i == 4) 
					nextTimeIdle[c] += std::stoi(wait);
				
				firstLine.erase(0, firstLine.find(delimiter)+ delimiter.length());
				wait.clear();
			}

			// usage = totalTime - idle
			nextTimeUsage[c] = nextTimeTotal[c] - nextTimeIdle[c];
			timeTotal[c] = nextTimeTotal[c] - currTimeTotal[c];
			timeUsage[c] = nextTimeUsage[c] - currTimeUsage[c];

			// calculate utilization
			if(nextTimeTotal[c] - currTimeTotal[c] >  0)
				nextUtilization[c] = timeUsage[c] / timeTotal[c]; 
				
		}
		
		// get max utilization ratio
		maxUtilization = nextUtilization[1];
		for(int i = 2; i < 5; i++){
			if(nextUtilization[i] > maxUtilization)
				maxUtilization = nextUtilization[i];
		}	


		if(currUtilization[0] != nextUtilization[0]){
			
			desiredFreq = 1.25 * MAX_FREQ * maxUtilization;
			
			//check for limitations
			if(desiredFreq > MAX_FREQ)
				nextFreq = MAX_FREQ;
			
			else if(desiredFreq < MIN_FREQ)
				nextFreq = MIN_FREQ;
			
			else {
				nextFreq = (desiredFreq-currentFreq) / SCALING_FACTOR;
				nextFreq *= SCALING_FACTOR;
				nextFreq += currentFreq;

				if(nextFreq < desiredFreq)
					nextFreq += SCALING_FACTOR;
				
			}

			setFrequency(nextFreq);
			std::cout << "Max Util: " << int(maxUtilization * 100) << " %" << "\t\t\t " << " Desired Freq: " 
				<< float(desiredFreq) / 1000000 << " MHz" << "\t\t\t " << "Current Freq: " 
				<< float(currentFreq) / 1000000 << " MHz" << "\t\t\t " << " %\r"; 
			std::cout.flush();
		}		
		
		currentFreq = getFrequency();
		
		// update variables
		for(int i = 0; i < 5; i++){
			currUtilization[i] = nextUtilization[i];
			currTimeTotal[i] = nextTimeTotal[i];	
			currTimeUsage[i] = nextTimeUsage[i];
		}

		this_thread::sleep_for(chrono::milliseconds(500));
	}
		
	return 0;
}
