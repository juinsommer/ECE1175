#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
using namespace std;

void setGovUser()
{
	ofstream scaling_governor;
	scaling_governor.open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor",ios::trunc);
	scaling_governor << "userspace\n";
	scaling_governor.close();
	cout << "Current Governor = " << "userspace\n";
}

int getFrequency()
{
	int freq;
	//Ifstream variable relating to the current frequency
	ifstream cpuinfo_cur_freq;
	cpuinfo_cur_freq.open("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq");
	//Read in from file and store into freq
	cpuinfo_cur_freq >> freq;
	//Close the file
	cpuinfo_cur_freq.close();
	cout << "Current Frequency = " << freq << endl;
	return freq;
}


void setFrequency(int freq)
{
	ofstream scaling_setspeed;
	scaling_setspeed.open("/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed",ios::trunc);
	//Writing freq to the file scaling_setspeed
	scaling_setspeed << freq;
	//Close the file
	scaling_setspeed.close();
	cout << "Set Frequency = " << freq << endl;
}

string read_proc_stat(int line)
{
	//Read file
	string startLine = "";
	ifstream proc_stat;
	//Set directory
	proc_stat.open("/proc/stat");
	//Read the file line by line
	for(int i = 0; i < line+1; i++)
	{
		//Get characters from stat, put them into startLine
		getline(proc_stat, startLine);
	}
	//Close the file
	proc_stat.close();
	return startLine;
}

int main()
{
	setGovUser();
	
	//set min frequency, max frequency, and scaling frequency
	
	const int frequency_min =  600000;
	const int frequency_max = 1500000;
	const int scaling_frequency = 100000; 
	
	//Array used to calculate utilization (used for storing numbers)
	
	long int timeTotal[5] = {0,0,0,0,0};
	long int timeUsage[5] = {0,0,0,0,0};
	long int currentFreq = getFrequency();
	double Utilization[5] = {0,0,0,0,0};
	//infinte loop
	while(true)
	{
		//declare variables
		long int newT_Total[5] = {0,0,0,0,0};
		long int newT_Usage[5] = {0,0,0,0,0};
		long int newT_Idle[5]  = {0,0,0,0,0};	
		long double  Utilization_max = 0;
		long int next_freq = getFrequency();
		long double desired_freq = 0;
		long double deltaTimeTotal[5] = {0,0,0,0,0};
		long double deltaTimeUsage[5] = {0,0,0,0,0};
		long double newUtilization[5] = {0,0,0,0,0};
		
		//i represents the number of CPUs
		for(int i = 0; i < 5; ++i)
		{	
			// read from proc/stat
			string startLine = read_proc_stat(i);
			
			//place where you stop in between info
			string stop_pt = " "; 
			//temporary variable to store value
			string temp = "";
			if(i == 0)
			{
				startLine.erase(0, startLine.find(stop_pt)+ stop_pt.length()+1);//erase first word
			}
			else
			{
				startLine.erase(0, startLine.find(stop_pt)+ stop_pt.length());
			}
			//8 represents the lines of data about time from /proc/stat
			for(int i = 0; i < 8; i++)
			{
				temp = startLine.substr(0, startLine.find(stop_pt));
				
				//since reading it as a string, need to convert to int to sum it up
				newT_Total[i] += stoi(temp);
				//3rd and 4th column contain idle times
				if(i == 3 || i == 4)
				{
					newT_Idle[i] += stoi(temp);
				}
				//erase part used
				startLine.erase(0, startLine.find(stop_pt)+ stop_pt.length());
				temp.clear();
			}
			//T_usage = T_total - T_idle
			newT_Usage[i] = newT_Total[i] - newT_Idle[i];
			//delta equations
			deltaTimeTotal[i] = newT_Total[i] - timeTotal[i];
			deltaTimeUsage[i] = newT_Usage[i] - timeUsage[i];
			//Utilization percentage
			if(newT_Total[i] - timeTotal[i] >  0)
				newUtilization[i] = deltaTimeUsage[i]/deltaTimeTotal[i]; 
				
		}
		//Find max utilization
		Utilization_max = newUtilization[1];
		for(int i = 2; i < 5; i++)
		{
			if(newUtilization[i] > Utilization_max)
			{
				Utilization_max = newUtilization[i];
			}	
		}	
		if(Utilization[0] != newUtilization[0])
		{
			//Calculate desired frequency
			desired_freq = 1.25 * frequency_max * Utilization_max;
			
			//check for limitations
			if(desired_freq > frequency_max)
			{
				next_freq = frequency_max;
			}
			else if(desired_freq < frequency_min)
			{
				next_freq = frequency_min;
			}
			else
			{
				next_freq = (desired_freq-currentFreq)/scaling_frequency;
				next_freq *= scaling_frequency;
				next_freq += currentFreq;
				if(next_freq < desired_freq)
				{
					next_freq += scaling_frequency;
				}
			}
			cout << "Max Utililzation: " << Utilization_max << endl;
			cout << "Desired Frequency: " << desired_freq << endl;
			cout << "Current Frequency: " << currentFreq << endl;
			setFrequency(next_freq);
		}		
		// set currentFreq
		currentFreq = getFrequency();
		
		//reset variables to new values
		for(int k = 0; k < 5; k++)
		{
			Utilization[k] = newUtilization[k];
			timeTotal[k] = newT_Total[k];	
			timeUsage[k] = newT_Usage[k];
		}
		this_thread::sleep_for(chrono::milliseconds(400));
	}
		
	return 0;
}
