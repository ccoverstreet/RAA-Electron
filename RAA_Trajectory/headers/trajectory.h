#pragma once

#include "payload.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>


void Payload_Trajectory_Calculator(Payload& payload) {
	/* This is a revised version of the trajectory calculator with
	 * emphasis for future development and increased accuracy. The
	 * first version has an issue with the handling of stage separations
	 * leading to inconsistent final masses depending on timestep
	 * sizes */

	// Environment Variables and constants
	double g = 9.81; //  m/s^2
	double rho_air = 1.225; // kg/m^3
	double Pi = 3.141592653589;

	// Setting Initial Conditions
	payload.time.push_back(0);
	payload.x_position.push_back(0);
	payload.y_position.push_back(0);
	payload.z_position.push_back(0);

	payload.x_velocity.push_back(0);
	payload.y_velocity.push_back(0);
	payload.z_velocity.push_back(0);

	// Temporary variables that change each calculation loop
	double current_x_position = 0;
	double current_y_position = 0;
	double current_z_position = 0;

	double current_x_velocity = 0;
	double current_y_velocity = 0;
	double current_z_velocity = 0;
	double current_speed = 0;

	double current_x_acceleration = 0;
	double current_y_acceleration = 0;
	double current_z_acceleration = 0;

	double current_thrust = 0;

	// Preparing drag constants
	double rocket_drag_constant = 0.5 * payload.rocket_cd * std::pow((0.5 * payload.rocket_diameter), 2) * Pi;
	double parachute_drag_constant = 0.5 * payload.parachute_cd * std::pow((0.5 * payload.parachute_diameter), 2) * Pi;
	double rocket_drag = 0;
	double parachute_drag = 0;
	double pressure_0 = 101325; // (Pa), atmospheric pressure at sea altitude_above_sea_level
	double temperature_0 = 288.15; // (K),sea level Temperature_0
	double temperature_lapse_rate = 0.0065; // (K/m) decrease in temperature with minHeight
	double ideal_gas_constant = 8.31447; // (J/mol-k) ideal gas constants
	double molar_mass_air = 0.0289644; // (kg/mol) molar mass of atmospheric molar_mass_air
	double temp_pressure = 0;
	double temp_density = 0;

	// Setting direction of the rocket
	double current_x_direction = std::cos(payload.vertical_direction * Pi / 180) * std::cos(payload.horizontal_direction * Pi / 180); // Calculating Direction from initial values
	double current_y_direction = std::cos(payload.vertical_direction * Pi / 180) * std::sin(payload.horizontal_direction * Pi / 180);
	double current_z_direction = std::sin(payload.vertical_direction * Pi / 180);

	double current_altitude = payload.starting_altitude; // Setting starting altitude in m

	double current_time = payload.timestep_size;
	double current_mass = payload.initial_mass + 0.5 * payload.stage_mass_rate_change[0];

	
	int parachute_status = 0; // 0 is non-deployed, 1 is deployed. Used to minimize comparisons during loop
	int launch_rail_status = 1; // 1 is on launch rail, 0 is off launch rail
	int stage_number = 0; // For incrementing stage arrays
	int stage_subnumber = 0; // 0 is burn phase, 1 is glide phase;

	// Main calculation loop
	while (current_z_position >= 0 ) {

		// Determining stage_number, stage_subnumber, and mass due to stage separations
		if (parachute_status != 1) {
			if (float(current_time) > payload.stage_firing_and_glide_times[stage_number][stage_subnumber]) {
				if (stage_subnumber == 0) { // The current stage is the burn stage
					current_mass += -0.5 * payload.stage_mass_rate_change[stage_number]; // Eliminating mass offset

					if (payload.stage_delay_time[stage_number] == 0) { // No delay after current burn stage
						if (payload.stage_separation[stage_number] == 1){ // Checking if stage separates
							current_mass += -payload.stage_dry_mass[stage_number];
						}	
						
						if (stage_number + 1 == payload.number_of_stages) { // Current stage is the last stage and has no delay
							parachute_status = 1; // Activating parachute
							current_thrust = 0; // No thrust during parachute phase
						} else {
							stage_number += 1; // Moving on to next stage
							current_mass += 0.5 * payload.stage_mass_rate_change[stage_number]; //Setting mass offset
						}	
					} else { // Delay after current burn stage
						stage_subnumber = 1; // Switching to delay stage
					}

				} else { // Current stage is the glide stage
					if (payload.stage_separation[stage_number] == 1) { // Current stage separates
							current_mass += -payload.stage_dry_mass[stage_number]; // Subtracting mass of stage
					}	
					
					if (stage_number + 1 == payload.number_of_stages) { // If current stage is last stage
						parachute_status = 1; // Deploying parachute
						current_thrust = 0; // No thrust during parachute phase
					} else {
						stage_subnumber = 0; // Setting stage subnumber back to burn stage
						stage_number += 1; // Moving to next stage
						current_mass += 0.5 * payload.stage_mass_rate_change[stage_number]; // Setting mass offset
					}
				}	
			}
		}

		// Setting thrust and changing mass as necessary. No action necessary for parachute phase as thrust is set to 0 automatically
		if (parachute_status != 1) { // Parachute is not deployed
			if (stage_subnumber == 0) { // Current stage is burn stage
				current_mass += -payload.stage_mass_rate_change[stage_number];
				current_thrust = payload.stage_average_force[stage_number]; 
			} else { // Current stage is glide stage
				current_thrust = 0;
			} 
		}	

		// Drag Force Calculations
		temp_pressure = pressure_0 * std::pow((1 - (temperature_lapse_rate * (payload.starting_altitude + payload.z_position.back())) / (temperature_0)), (g * molar_mass_air) / (ideal_gas_constant * temperature_lapse_rate));
		temp_density = (temp_pressure * molar_mass_air) / (ideal_gas_constant * (temperature_0 - temperature_lapse_rate * payload.z_position.back())); //  calculated density at current altitude

		rocket_drag = 1 * rocket_drag_constant * temp_density * std::pow(current_speed, 2); // calculating rocket Drag
		if (parachute_status == 1){
			parachute_drag = 1 * parachute_drag_constant * temp_density * std::pow(current_speed, 2); // calculating parachute Drag
		}

		// Acceleration Calculations
		current_x_acceleration = (current_thrust - parachute_drag - rocket_drag) * payload.timestep_size * current_x_direction * (1 / current_mass);
		current_y_acceleration = (current_thrust - parachute_drag - rocket_drag) * payload.timestep_size * current_y_direction * (1 / current_mass);
		current_z_acceleration = (current_thrust * current_z_direction - parachute_drag * current_z_direction - rocket_drag * current_z_direction - g * current_mass) * payload.timestep_size * (1 / current_mass);

		// Velocity Calculations
		current_x_velocity += current_x_acceleration;
		current_y_velocity += current_y_acceleration;
		current_z_velocity += current_z_acceleration;

		// Calculating currrent speed
		current_speed = std::sqrt(std::pow(current_x_velocity, 2) + std::pow(current_y_velocity, 2) + std::pow(current_z_velocity, 2)); 

		// Position Calculations
		current_x_position += 0.5 * (current_x_velocity + payload.x_velocity.back()) * payload.timestep_size;
		current_y_position += 0.5 * (current_y_velocity + payload.y_velocity.back()) * payload.timestep_size;
		current_z_position += 0.5 * (current_z_velocity + payload.z_velocity.back()) * payload.timestep_size;

		// Determining direction. Direction does not change if the rocket is still on the launch rail
		if (launch_rail_status != 1){
			current_x_direction = current_x_velocity / current_speed;
			current_y_direction = current_y_velocity / current_speed;
			current_z_direction = current_z_velocity / current_speed;
		} else{
			if (std::sqrt(std::pow(current_x_position, 2) + std::pow(current_y_position, 2) + std::pow(current_z_position, 2)) > payload.launch_rail_length){
				launch_rail_status = 0;
			}
		}

		// Data storage
		payload.x_position.push_back(current_x_position);
		payload.y_position.push_back(current_y_position);
		payload.z_position.push_back(current_z_position);

		payload.x_velocity.push_back(current_x_velocity);
		payload.y_velocity.push_back(current_y_velocity);
		payload.z_velocity.push_back(current_z_velocity);

		payload.x_acceleration.push_back(current_x_acceleration / (payload.timestep_size * g));
		payload.y_acceleration.push_back(current_y_acceleration / (payload.timestep_size * g));
		payload.z_acceleration.push_back(current_z_acceleration / (payload.timestep_size * g));

		payload.half_times.push_back((current_time - 0.5 * payload.timestep_size));

		payload.time.push_back(current_time);
	
		
		// Continue to next time step
		current_time += payload.timestep_size;
	}

	payload.final_mass = current_mass; // Storing final mass of rocket. Mostly for debugging and logic checks
	
}



