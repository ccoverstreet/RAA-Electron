#include <iostream>
#include <map>
#include <string>
#include <stdlib.h>

#include "headers/payload.h"

// Initializing Payload Object
Payload rocket;

std::map<std::string, void (*)()> function_map;

void exit() {
	std::cout << "Exiting RAA Trajectory\n";
	exit(0);
}

void listpayload() {
	rocket.Print_Payload_Initial_Parameters();
	std::cout << "\n\n";
}

void commands() {
	std::cout << std::endl;
	for (auto it = function_map.cbegin(); it != function_map.cend(); it++) {
		std::cout << it -> first << std::endl;
	}
	std::cout << std::endl;
}

void editparameters() {
	while(true) {
		std::cout << "[RAA-Trajectory parameters]";
	}
}

int main() {
	// Reading values from passed_parameters.json
	rocket.Read_Parameter_File();

	// Printing starting information
	std::cout << "\n\nCommand Line Interface for RAA Trajectory\n";

	function_map["exit"] = &exit;
	function_map["listpayload"] = &listpayload;
	function_map["commands"] = &commands;

	std::string user_input;
	
	// Main Program Loop
	while (true) {
		std::cout << "[RAA Trajectory]: ";
		std::cin >> user_input;
		if (function_map.find(user_input) == function_map.end()) {
			// Function not in function map
		} else {
			// Function in function map. Executing function with user_input
			function_map[user_input]();
		}
	}

	return 0;
}
