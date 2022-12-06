#include <iostream>
#include <fstream>
#include <string>

void setGovernor(std::string governor) {
    std::ofstream scaling_governor;
	scaling_governor.open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor",std::ios::trunc);

	if(scaling_governor.is_open()) {
		scaling_governor << governor;
		scaling_governor.close();
	
		std::cout << "\nCurrent Governor: " << governor << std::endl; 
	}		
	
	else
		std:: cout << "\nError change_gov.cpp: No governor selected." << std::endl;
}

int getFrequency() {
	int frequency;
	std::ifstream cpuinfo_cur_freq;
	cpuinfo_cur_freq.open("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq");

	if(cpuinfo_cur_freq.is_open()) {
		cpuinfo_cur_freq >> frequency;
		cpuinfo_cur_freq.close();
	}

	else
		std::cout << "\nError int getFrequency: Cannot open file" << std::endl;

	return frequency;
}

void vgetFrequency() {
	std::string frequency;
	std::ifstream cpuinfo_cur_freq;
	cpuinfo_cur_freq.open("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq");
	
	if(cpuinfo_cur_freq.is_open()) {
		cpuinfo_cur_freq >> frequency;
		cpuinfo_cur_freq.close();
		std::cout << "Current frequency: " << std::stoi(frequency) / double(1000000) << "MHz" << std::endl;;
	}

	else
		std::cout << "Error vgetFrequency: Cannot open /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq" << std::endl;
}


void setFrequency(int frequency) {
	std::ofstream scaling_setspeed;
	scaling_setspeed.open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed",std::ios::trunc);

 	if(scaling_setspeed.is_open()) { 
		scaling_setspeed << frequency;
		scaling_setspeed.close();
 
		std::cout << "Set Frequency:  " << frequency / double(1000000) << " MHz\t\t\t "; 
	}
	
	else
		std::cout <<"\nNo frequency set." << std::endl;
}


std::string readProcStat(int line) {
	std::string firstLine = "";
	std::ifstream stat;
	stat.open("/proc/stat");

	for(int i = 0; i < line + 1; i++)
		std::getline(stat, firstLine);
	
	stat.close();
	
	return firstLine;
}

void runWorkload() {
	int result = 0;
	const long int repeating_time = 500000000;

	for (long int i=0; i<repeating_time; i++) {
		if(result > 1000)
			result -= 1;
		else
			result += 1;
	}
}
