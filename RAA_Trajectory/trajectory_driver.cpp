#include "headers/payload.h"
#include "headers/trajectory.h"
#include <iostream>
#include <iomanip>
#include <chrono>


int main() {
	/* Main driver and entry point for the trajectory calculations
	   used. This will be designed for use in both the cluster and 
	   the mission control */

		
	auto start = std::chrono::steady_clock::now(); //Getting program start time
	
	Payload rocket1;
	rocket1.Read_Parameter_File();
	rocket1.Print_Payload_Initial_Parameters();
	Payload_Trajectory_Calculator(rocket1);
	rocket1.Print_Payload_Results();
	rocket1.Write_Data_to_CSV();
	rocket1.Write_to_JSON();

	auto end = std::chrono::steady_clock::now(); //Getting program end time

	//Calculating time spent
	std::cout << "\n\nProgram Performance:";
	std::cout << "\n\tElapsed Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us";
	
	//Adding spaces before program exits
	printf("\n\n");

	return 0;
}
