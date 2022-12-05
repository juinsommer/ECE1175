#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
using namespace std;

void setGovUser(){
	//make scaling_governor the userspace
	//open scaling_governer write userspace to file and close
	//error message if it can't be opened
	ofstream scaling_governor;
	scaling_governor.open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor",ios::trunc);
	if(scaling_governor.is_open()){
		scaling_governor << "userspace\n";
		scaling_governor.close();
	}else{
		cout << "ERROR: scaling_governor can't be opened.\n";
	}
	cout << "CURRENT GOV = " << "userspace\n";
}

int getFreq(){
	//open cpuinfo_cur_freq read the current freq and close
	//error message if it can't be opened
	int f;
	ifstream cpuinfo_cur_freq;
	cpuinfo_cur_freq.open("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq");
	if(cpuinfo_cur_freq.is_open()){
		cpuinfo_cur_freq >> f;
		cpuinfo_cur_freq.close();
	}else{
		cout << "ERROR: cpuinfo_cur_freq can't be opened.\n";
	}
	//cout << "CURRENT F = " << f << endl;
	return f;
}


void setFreq(int f){
	//open scaling_setspeed for writing cpu freq
	//write new freq to be set and close
	//error message if it can't be opened
	ofstream scaling_setspeed;
	scaling_setspeed.open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed",ios::trunc);
	if(scaling_setspeed.is_open()){
		scaling_setspeed << f;
		scaling_setspeed.close();
	}else{
		cout << "ERROR: scaling_setspeed can't be opened.\n";
	}
	cout << "SET F = " << f << endl;
}


string readStat(int lineToRead ){
	string firstLine = "";
	ifstream stat;
	stat.open("/proc/stat");
	if(stat.is_open()){
		for(int i = 0; i < lineToRead+1; i++){
			getline(stat,firstLine);
		}
		stat.close();
	}else{	
		cout << "ERROR: stat can't be opened.\n";
	}
	return firstLine;
}

int main(){
	setGovUser();
	//set constants for this rpi
	const int MIN_FREQ =  600000;
	const int MAX_FREQ = 1500000;
	const int SCALINGF =  100000; 
	//set variables to be reused
	long int oldTimeTotal[5] = {0,0,0,0,0};
	long int oldTimeUsage[5] = {0,0,0,0,0};
	long int currentFreq = getFreq();
	long double oldUtilization[5] = {0,0,0,0};

	while(true){
		long int newTimeTotal[5] = {0,0,0,0,0};
		long int newTimeUsage[5] = {0,0,0,0,0};
		long int newTimeIdle[5]  = {0,0,0,0,0};	
		long double  maxUtil = 0;
		long int newFreq = getFreq();
		long double desired = 0;
		long double changeTimeTotal[5] = {0,0,0,0,0};
		long double changeTimeUsage[5] = {0,0,0,0,0};
		long double newUtilization[5] = {0,0,0,0,0};
		

		for(int cpu = 0; cpu < 5; ++cpu){	
			//get proc stat information
			string firstLine = readStat(cpu);
			
			//sum values
			//
			//
			
			string delimiter = " ";//use space as delimiter
			string token = "";//variable to hold value
			if(cpu == 0){
				firstLine.erase(0, firstLine.find(delimiter)+ delimiter.length()+1);//erase first word
			}else{
				firstLine.erase(0, firstLine.find(delimiter)+ delimiter.length());
			}

			for(int i = 0; i < 8; i++){//parse through string until empty
				//hold value
				token = firstLine.substr(0, firstLine.find(delimiter));
				//cout << "\nTOKEN: " << token << "\n";
				
				//convert value to int and add to sum
				newTimeTotal[cpu] += stoi(token);
				//cout << "\n" << "newTimeTotal: " << newTimeTotal << "\n";
				//third and fourth column has idle
				if(i == 3 || i == 4){
					newTimeIdle[cpu] += stoi(token);
					//cout << "\n" << "newTimeIdle: "  << newTimeIdle << "\n";
				}
				//erase part used
				firstLine.erase(0, firstLine.find(delimiter)+ delimiter.length());
				token.clear();
			}

			//usage = total - idle
			newTimeUsage[cpu] = newTimeTotal[cpu] - newTimeIdle[cpu];
			//cout << "\n" << "newTimeUsage: "  << newTimeUsage << "\n";
		
			//calc deltas to proper variables
			changeTimeTotal[cpu] = newTimeTotal[cpu] - oldTimeTotal[cpu];
			changeTimeUsage[cpu] = newTimeUsage[cpu] - oldTimeUsage[cpu];


			//util = delta Usage / delta Total
			//
			if(newTimeTotal[cpu] - oldTimeTotal[cpu] >  0)
				newUtilization[cpu] = changeTimeUsage[cpu]/changeTimeTotal[cpu]; 
				
		}
		
		//find highest util ratio
		//
		maxUtil = newUtilization[1];
		for(int i = 2; i < 5; i++){
			if(newUtilization[i] > maxUtil){
				maxUtil = newUtilization[i];
			}	
		}	


		if(oldUtilization[0] != newUtilization[0]){
			//if utilaztion changed change fre
			//
			
			desired = 1.25 * MAX_FREQ * maxUtil;
			
			//check for limitations
			if(desired > MAX_FREQ){
				newFreq = MAX_FREQ;
			}else if(desired < MIN_FREQ){
				newFreq = MIN_FREQ;
			}else{
				//increment by 100000(SCALINGF)
				//find difference
				//divide and multiply on seperate lines to Make value proper scaling
				//add the current to the difference for the resulting new frequency
				//if lower give it one step up for ceiling math
				newFreq = (desired-currentFreq)/SCALINGF;
				newFreq *= SCALINGF;
				newFreq += currentFreq;
				if(newFreq < desired){
					newFreq += SCALINGF;
				}
			}
			cout << "MaxUtil: " << maxUtil << endl;
			cout << "DESIRED: " << desired << endl;
			cout << "Curr Freq: " << currentFreq << endl;
			//give cpu command to change freq
			setFreq(newFreq);
					
		}		
		
		// set current Freq
		currentFreq = getFreq();
		
		//set old variables to new
		for(int k = 0; k < 5; k++){
			oldUtilization[k] = newUtilization[k];
			oldTimeTotal[k] = newTimeTotal[k];	
			oldTimeUsage[k] = newTimeUsage[k];
		}
		this_thread::sleep_for(chrono::milliseconds(400));
	}
		
	return 0;
}
