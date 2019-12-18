#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <cmath>
#include "json.hpp"
using json = nlohmann::json;

class Payload {
	public:
		//Initial Conditions
		double timestep_size;
		double horizontal_direction;
		double vertical_direction;
		double starting_altitude;
		double launch_rail_length;
		double initial_mass;

		//Rocket Characteristics
		double rocket_diameter;
		double rocket_cd;
		double parachute_diameter;
		double parachute_cd;
		double number_of_stages;
		std::vector<double> stage_impulse;
		std::vector<double> stage_average_force;
		std::vector<double> stage_burn_time;
		std::vector<double> stage_delay_time;
		std::vector<double> stage_total_mass;
		std::vector<double> stage_dry_mass;
		std::vector<double> stage_mass_rate_change;
		std::vector<int> stage_separation;
		std::vector<std::vector<double>> stage_firing_and_glide_times;
		double parachute_deployment_time;

		//OUTPUTS
		double final_mass;
		std::vector<double> time;
		std::vector<double> half_times;

		std::vector<double> x_velocity;
		std::vector<double> y_velocity;
		std::vector<double> z_velocity;

		std::vector<double> x_acceleration ;
		std::vector<double> y_acceleration;
		std::vector<double> z_acceleration;

		std::vector<double> x_position;
		std::vector<double> y_position;
		std::vector<double> z_position;

		//Payload functions
		void Print_Payload_Initial_Parameters();	

		void Read_Parameter_File();

		void Print_Payload_Results();

		void Write_Data_to_CSV();

		void Write_to_JSON();
};

void Payload::Read_Parameter_File() {
	/*Reads parameter file passed from RAA_Electron and stores values inside the
	  instance of the payload class*/

	printf("\n\nReading parameters passed from model_parameters.json..."); 
	//opening and reading json file
	std::ifstream raw("passed_parameters.json");
	json input;
	raw >> input;

	//storing values in C++ form
	timestep_size = double(input["timestep_size"]);
	horizontal_direction = double(input["horizontal_direction"]);
	vertical_direction = double(input["vertical_direction"]);
	starting_altitude = double(input["starting_altitude"]);
	launch_rail_length = double(input["launch_rail_length"]);
	initial_mass = double(input["initial_mass"]);
	rocket_diameter = double(input["rocket_diameter"]);
	rocket_cd = double(input["rocket_cd"]);
	parachute_diameter = double(input["parachute_diameter"]);
	parachute_cd = double(input["parachute_cd"]);
	number_of_stages = int(input["number_of_stages"]);

	json temp_stage_impulse = input["stage_impulse"];
	json temp_stage_burn_time = input["stage_burn_time"];
	json temp_stage_delay_time = input["stage_delay_time"];
	json temp_stage_total_mass = input["stage_total_mass"];
	json temp_stage_dry_mass = input["stage_dry_mass"];
	json temp_stage_separation = input["stage_separation"];

	for (int i = 0; i < number_of_stages; i++) {
		stage_impulse.push_back(double(temp_stage_impulse.at(i)));
		stage_burn_time.push_back(double(temp_stage_burn_time.at(i)));
		stage_average_force.push_back(stage_impulse[i] / stage_burn_time[i]);
		stage_delay_time.push_back(double(temp_stage_delay_time.at(i)));
		stage_total_mass.push_back(double(temp_stage_total_mass.at(i)));
		stage_dry_mass.push_back(double(temp_stage_dry_mass.at(i)));
		stage_separation.push_back(int(temp_stage_separation.at(i)));
		double temp_mass_rate = ((stage_total_mass[i] - stage_dry_mass[i]) / (stage_burn_time[i])) * (timestep_size);
		stage_mass_rate_change.push_back(temp_mass_rate);
	}

	int m = int(number_of_stages), n = 2;
	stage_firing_and_glide_times.resize(m);
	for(int i = 0 ; i < m ; ++i){
		//Grow rows by n
		stage_firing_and_glide_times[i].resize(n);
	}

	double temp_buffer_time = 0;
	for (int i = 0; i < number_of_stages; i++){
		temp_buffer_time += stage_burn_time[i];
		stage_firing_and_glide_times[i][0] = temp_buffer_time;
		temp_buffer_time += stage_delay_time[i];
		stage_firing_and_glide_times[i][1] = temp_buffer_time;
	}


	parachute_deployment_time = temp_buffer_time;
}

void Payload::Print_Payload_Initial_Parameters(){ //Redo this function to use printf for efficiency
	/*This prints out the initial data loaded from the passed_parameters.json
	  written by RAA_Electron*/

	//Object Components
	double object_components[] = {
		timestep_size,
		horizontal_direction,
		vertical_direction,
		starting_altitude,
		launch_rail_length,
		initial_mass,
		rocket_diameter,
		rocket_cd,
		parachute_diameter,
		parachute_cd,
		number_of_stages
	};

	//Names for print loop
	std::string object_component_names[] = {
		"Timestep Size (s)",
		"Horizontal Direction (deg)",
		"Vertical Direction (deg)",
		"Starting Altitude (m)",
		"Launch Rail Length (m)",
		"Initial Mass (kg)",
		"Rocket Diameter (m)",
		"Rocket CD",
		"Parachute Diameter (m)",
		"Parachute CD",
		"Number of Stages"
	};

	for (int i = 0; i < (*(&object_components + 1) - object_components); i++){
		std::cout << "\n\t" << object_component_names[i] << ": " << object_components[i];
	}

	std::cout << "\n\n\tStage Impulses (N-s): ";
	for (int i = 0; i < number_of_stages; i++){
		if (i != 0){
			std::cout << ", ";
		}
		std::cout << stage_impulse[i];
	}

	std::cout << "\n\tStage Burn Time (s): ";
	for (int i = 0; i < number_of_stages; i++){
		if (i != 0){
			std::cout << ", ";
		}
		std::cout << stage_burn_time[i];
	}

	std::cout << "\n\tStage Average Force (N): ";
	for (int i = 0; i < number_of_stages; i++){
		if (i != 0){
			std::cout << ", ";
		}
		std::cout << stage_average_force[i];
	}

	std::cout << "\n\tStage Mass Rate of Change (kg/s): ";
	for (int i = 0; i < number_of_stages; i++){
		if (i != 0){
			std::cout << ", ";
		}
		std::cout << stage_mass_rate_change[i];
	}

	std::cout << "\n\tStage Delay Time (s): ";
	for (int i = 0; i < number_of_stages; i++){
		if (i != 0){
			std::cout << ", ";
		}
		std::cout << stage_delay_time[i];
	}

	std::cout << "\n\tStage Total Mass (kg): ";
	for (int i = 0; i < number_of_stages; i++){
		if (i != 0){
			std::cout << ", ";
		}
		std::cout << stage_total_mass[i];
	}

	std::cout << "\n\tStage Dry Mass (kg): ";
	for (int i = 0; i < number_of_stages; i++){
		if (i != 0){
			std::cout << ", ";
		}
		std::cout << stage_dry_mass[i];
	}

	std::cout << "\n\tStage Separation: ";
	for (int i = 0; i < number_of_stages; i++){
		if (i != 0){
			std::cout << ", ";
		}
		std::cout << stage_separation[i];
	}

	std::cout << "\n\tStage Event Times (s): ";
	for (int i = 0; i < number_of_stages; i++){
		std::cout << "\n\t\t\t ";
		for (int j = 0; j < 2; j++){
			if (j == 0 && i == 0){
				std::cout << "Stage Firing Times (s): ";
			} else if(j == 0 && i == 1){
				std::cout << "Stage Glide Times  (s): ";
			} else{
				std::cout << ", ";
			}
			std::cout << stage_firing_and_glide_times[i][j];
		}
	}


	std::cout << "\n\tParachute Deployment Time (s): " <<  parachute_deployment_time;
}

void Payload::Print_Payload_Results(){
	std::cout << "\n\nPrinting Results...";
	std::cout << "\n\tTotal Flight Time (s): " << time.back();
	std::cout << "\n\tApogee (m): " << *std::max_element(z_position.begin(), z_position.end());
	std::cout << "\n\tTime of Apogee (s): " << time.at(std::max_element(z_position.begin(), z_position.end()) - z_position.begin());
	std::cout << "\n\tX-Position (m): " << x_position.back();
	std::cout << "\n\tY-Position (m): " << y_position.back();

	std::cout << "\n\tHorizontal Distance (m): " << std::sqrt(std::pow(x_position.back(), 2) + std::pow(y_position.back(), 2));
	std::cout << "\n\tLanding Velocity (m/s): " << std::sqrt(std::pow(x_velocity.back(), 2) + std::pow(y_velocity.back(), 2) + std::pow(z_velocity.back(), 2));
	std::cout << "\n\tFinal Mass (kg): " << final_mass;
}

void Payload::Write_Data_to_CSV(){

	std::ofstream output_data;
	output_data.open("output_data.csv");
	output_data << "time,payload_x_position,payload_y_position,payload_z_position,payload_x_velocity,payload_y_velocity,payload_z_velocity,half_times,payload_x_acceleration,payload_y_acceleration,payload_z_acceleration\n";

	for (int i = 0; i < time.size(); i++){
		if (i < time.size() -1){
			output_data << time.at(i) << "," << x_position.at(i) << "," << y_position.at(i) << "," << z_position.at(i) << "," << x_velocity.at(i) << "," << y_velocity.at(i) << "," << z_velocity.at(i) << "," << half_times.at(i) << "," << x_acceleration.at(i) << "," << y_acceleration.at(i) << "," << z_acceleration.at(i) << "\n";
		} else{
			output_data << time.at(i) << "," << x_position.at(i) << "," << y_position.at(i) << "," << z_position.at(i) << "," << x_velocity.at(i) << "," << y_velocity.at(i) << "," << z_velocity.at(i) << "\n";
		}
	}

	output_data.close();
}

void Payload::Write_to_JSON(){
	json output;

	output["payload_total_flight_time"] = time.back();
	output["payload_apogee"] = *std::max_element(z_position.begin(), z_position.end());
	output["payload_time_of_apogee"] = time.at(std::max_element(z_position.begin(), z_position.end()) - z_position.begin());
	output["payload_final_x_position"] = x_position.back();
	output["payload_final_y_position"] = y_position.back();
	output["payload_horizontal_distance"] = std::sqrt(std::pow(x_position.back(), 2) + std::pow(y_position.back(), 2));
	output["payload_landing_velocity"] = std::sqrt(std::pow(x_velocity.back(), 2) + std::pow(y_velocity.back(), 2) + std::pow(z_velocity.back(), 2));
	output["payload_final_mass"] = final_mass;

	std::ofstream json_output;
	json_output.open("output_values.json");
	json_output << std::setw(4) << output << std::endl;
	json_output.close();
}
