//GUI Functions for RAA electron
//Cale Overstreet and RAA
/* This is the javascript file for the index.html file that contains the html
elements for the GUI. This file is written/to be written in as much pure
javascript as possible */

//Global Variables
var number_of_stages = 1;
var OSName = "";
var plot3d_enabled = 1; //1 is for enabled, 0 is for disabled. Keep disabled for low performing devices


function Detect_OS() {

	if (navigator.appVersion.indexOf("Win")!=-1) OSName="Windows";
	if (navigator.appVersion.indexOf("Mac")!=-1) OSName="MacOS";
	if (navigator.appVersion.indexOf("X11")!=-1) OSName="UNIX";
	if (navigator.appVersion.indexOf("Linux")!=-1) OSName="Linux"
	console.log('Your operating system was identified as ' + OSName)
}

function Compile_CPP() {
	var { spawn } = require('child_process');
	var child = spawn('node setup_scripts/compile_cpp.js', {
		stdio: 'inherit',
		shell: true
	});
}

//Reads settings.json and sets values
function Read_Settings() {
	const fs = require('fs');
	let rawdata = fs.readFileSync('settings.json');
	let settings_content = JSON.parse(rawdata);

	plot2d_status = settings_content.plot_2d_status;
	plot3d_status = settings_content.plot_3d_status;
}

//Controls number of stages and display
function addStage() {
	number_of_stages += 1;

	//Cloning Stage Template which is Stage 1
	var stage = document.getElementById("stage_1");
	var new_stage = stage.cloneNode(true);

	//Modifying the newly created stages attributes and IDs
	new_stage.id = "stage_" + number_of_stages;
	new_stage.querySelectorAll('[id="stage_name"]')[0].innerHTML = "Stage " + number_of_stages;
	new_stage.querySelectorAll('[id="stage_impulse_input_1"]')[0].id = "stage_impulse_input_" + number_of_stages;
	new_stage.querySelectorAll('[id="stage_burn_time_input_1"]')[0].id = "stage_burn_time_input_" + number_of_stages;
	new_stage.querySelectorAll('[id="stage_delay_time_input_1"]')[0].id = "stage_delay_time_input_" + number_of_stages;
	new_stage.querySelectorAll('[id="stage_total_mass_input_1"]')[0].id = "stage_total_mass_input_" + number_of_stages;
	new_stage.querySelectorAll('[id="stage_dry_mass_input_1"]')[0].id = "stage_dry_mass_input_" + number_of_stages;
	new_stage.querySelectorAll('[id="stage_separation_input_1"]')[0].checked = false;
	new_stage.querySelectorAll('[id="stage_separation_input_1"]')[0].id = "stage_separation_input_" + number_of_stages;

	//setting previous stage to Separate
	document.getElementById("stage_separation_input_" + (number_of_stages - 1)).checked = true;

	document.getElementById("stage_container").appendChild(new_stage); //Adding Stage to Stage Container

	//document.getElementById("stage_separation_input_" + (number_of_stages)).checked = false;
	console.log('Added Stage ' + number_of_stages);
}

//Removes stages
function removeStage() {
	if (number_of_stages == 1){
		console.log('Cannot have less than one stage');
		return;
	}

	//adjusting stage separation checkboxes
	document.getElementById("stage_separation_input_" + (number_of_stages - 1)).checked = false;

	var element = document.getElementById("stage_" + number_of_stages); //grabbing last stage element
	element.parentNode.removeChild(element); //removing element
	number_of_stages += -1; //decrementing stage number


	console.log('Removed Stage ' + (number_of_stages + 1));
}

//Switches pages
function switchTab(evt, pageName) {
	var i, tabcontent, tablinks;

	tabcontent = document.getElementsByClassName("tabcontent");
	for (i = 0; i < tabcontent.length; i++) {
		tabcontent[i].style.display = "none";
	}

	tablinks = document.getElementsByClassName("tablinks");
	for (i = 0; i < tablinks.length; i++) {
		tablinks[i].className =  tablinks[i].className.replace(" menu_button_active", "");
	}

	document.getElementById(pageName + "_Button").classList.add("menu_button_active");
	document.getElementById(pageName).style.display = "grid";
}

function switchOutput(evt, plotName) {
	var Plotly = require('plotly.js-dist');
	var i, outputContent, outputLinks;

	outputContent = document.getElementsByClassName("output_content");
	for(i = 0; i < outputContent.length; i++) {
		outputContent[i].style.display = "none";
	}

	outputLinks = document.getElementsByClassName("predicted_trajectory_link");
	for (i = 0; i < outputLinks.length; i++) {
		outputLinks[i].className = outputLinks[i].className.replace(" predicted_trajectory_button_active", "");
	}

	document.getElementById(plotName).style.display = "grid";
	document.getElementById(plotName + "_Button").classList.add("predicted_trajectory_button_active"); 
	Plotly.Plots.resize(document.getElementById(plotName));
}

//Collecting values from user input
function collect_model_parameters() {
	var fs = require('fs');

	if (document.getElementById("plot3d_enabled").checked == true) {
		plot3d_enabled = 1;
	} else {
		plot3d_enabled = 0;
	}

	const timestep_size = parseFloat(document.getElementById('timestep_size_input').value);
	const horizontal_direction = parseFloat(document.getElementById('horizontal_direction_input').value);
	const vertical_direction = parseFloat(document.getElementById('vertical_direction_input').value);
	const starting_altitude = parseFloat(document.getElementById('starting_altitude_input').value);
	const launch_rail_length = parseFloat(document.getElementById('launch_rail_length_input').value);
	const initial_mass = parseFloat(document.getElementById('initial_mass_input').value);
	const rocket_diameter = parseFloat(document.getElementById('rocket_diameter_input').value);
	const rocket_cd = parseFloat(document.getElementById('rocket_cd_input').value);
	const parachute_diameter = parseFloat(document.getElementById('parachute_diameter_input').value);
	const parachute_cd = parseFloat( document.getElementById('parachute_cd_input').value);
	var testArray = [1, 2, 3, 4];

	//Collecting data from stages and storing in arrays
	var stage_impulse = [];
	var stage_burn_time = [];
	var stage_delay_time = [];
	var stage_total_mass = [];
	var stage_dry_mass = [];
	var stage_separation = [];
	var i;
	for (i = 0; i < number_of_stages; i++) { //iterating through each stage and grabbing all values
		document.getElementById('stage_impulse_input_' + (i + 1)).value;
		stage_impulse.push(parseFloat(document.getElementById('stage_impulse_input_' + (i + 1)).value));
		stage_burn_time.push(parseFloat(document.getElementById('stage_burn_time_input_' + (i + 1)).value));
		stage_delay_time.push(parseFloat(document.getElementById('stage_delay_time_input_' + (i + 1)).value));
		stage_total_mass.push(parseFloat(document.getElementById('stage_total_mass_input_' + (i + 1)).value));
		stage_dry_mass.push(parseFloat(document.getElementById('stage_dry_mass_input_' + (i + 1)).value));
		if (document.getElementById('stage_separation_input_' + (i + 1)).checked == true) {
			stage_separation.push(1);
		} else {
			stage_separation.push(0);
		}
	}


	var passed_parameters = {
		"timestep_size": timestep_size,
		"horizontal_direction": horizontal_direction,
		"vertical_direction": vertical_direction,
		"starting_altitude": starting_altitude,
		"launch_rail_length": launch_rail_length,
		"initial_mass": initial_mass,
		"rocket_diameter": rocket_diameter,
		"rocket_cd": rocket_cd,
		"parachute_diameter": parachute_diameter,
		"parachute_cd": parachute_cd,
		"number_of_stages": number_of_stages,
		"stage_impulse": stage_impulse,
		"stage_burn_time": stage_burn_time,
		"stage_delay_time": stage_delay_time,
		"stage_total_mass": stage_total_mass,
		"stage_dry_mass": stage_dry_mass,
		"stage_separation": stage_separation
	};


	if (OSName == "Linux") {
		fs.writeFileSync("./passed_parameters.json", JSON.stringify(passed_parameters, null, "\t"), (err) => {
			if (err) {
				console.error(err);
				return;
			};
			console.log("File has been created");
		});
	} else {
		fs.writeFileSync("passed_parameters.json", JSON.stringify(passed_parameters, null, "\t"), (err) => {
			if (err) {
				console.error(err);
				return;
			};
			console.log("File has been created");
		});
	}
}

//Spawns subprocess for calculating trajectory, writing, and plotting data
function Trajectory_Calculator() {
	collect_model_parameters();

	if (OSName == "Linux") {
		var { spawn } = require('child_process');
		var child = spawn('./executables/trajectory_driver', {
			stdio: 'inherit',
			shell: true
		});
	} else if (OSName == "Windows") {
		var { spawn } = require('child_process');
		var child = spawn('executables/trajectory_driver.exe', {
			stdio: 'inherit',
			shell: false 
		});
	}

	//Listener function that reports back when child process has closed
	child.on('exit', function(code, signal) {
		Plot_Data();
		Print_Output();
		switchTab(event, 'Predicted_Trajectory');
	});
}

//Plots output of Trajectory executable
function Plot_Data() {
	var Plotly = require('plotly.js-dist');

	Plotly.purge('plot_2d_position');
	Plotly.purge('plot_3d_position');
	Plotly.purge('plot_2d_velocity');
	Plotly.purge('plot_2d_acceleration');

	Position_2D = document.getElementById('plot_2d_position');
	Velocity_2D = document.getElementById('plot_2d_velocity');
	Position_3D = document.getElementById('plot_3d_position');
	Acceleration_2D = document.getElementById('plot_2d_acceleration');

	Plotly.d3.csv('output_data.csv', function(err, rows){
		function unpack(rows, key) {
			return rows.map(function(row)
				{ return row[key]; }); }

		var time = unpack(rows , 'time');

		var payload_x_position = unpack(rows, 'payload_x_position');
		var payload_z_position = unpack(rows, 'payload_z_position');
		var payload_y_position = unpack(rows, 'payload_y_position');

		var payload_x_velocity = unpack(rows, 'payload_x_velocity');
		var payload_y_velocity = unpack(rows, 'payload_y_velocity');
		var payload_z_velocity = unpack(rows, 'payload_z_velocity');

		var half_times = unpack(rows, 'half_times');

		var payload_x_acceleration = unpack(rows, 'payload_x_acceleration');
		var payload_y_acceleration = unpack(rows, 'payload_y_acceleration');
		var payload_z_acceleration = unpack(rows, 'payload_z_acceleration');

		//3D Plot Section
		if (plot3d_enabled == 1) {
			var layout_position_3D = {
				margin: {
					t: 10,
					b: 10
				}
			};

			var payload_position_3d_trace = {
				type: 'scatter3d',
				mode: 'lines',
				x: payload_x_position,
				y: payload_y_position,
				z: payload_z_position
			};

			var data_position_3d = [payload_position_3d_trace];

			Plotly.plot(Position_3D,
				data_position_3d,
				layout_position_3D	
			);
		}

		//2D Position Plot
		var x_position_trace = {
			x: time,
			y: payload_x_position,
			type: 'scatter',
			name: 'x-position'
		};

		var y_position_trace = {
			x: time,
			y: payload_y_position,
			type: 'scatter',
			name: 'y-position'
		};

		var z_position_trace = {
			x: time,
			y: payload_z_position,
			type: 'scatter',
			name: 'z-position'
		};

		var data_position_2d = [x_position_trace, y_position_trace, z_position_trace];

		var layout_position_graph = {
			xaxis: {
				title: 'Time (s)'
			},
			yaxis: {
				title: 'Position (m)'
			},
			margin: {
				t: 10
			}
		};

		Plotly.plot(Position_2D,
			data_position_2d,
			layout_position_graph			
		);


		//2D Velocity Plot
		var x_velocity_trace = {
			x: time,
			y: payload_x_velocity,
			type: 'scatter',
			name: 'x-velocity'
		};

		var y_velocity_trace = {
			x: time,
			y: payload_y_velocity,
			type: 'scatter',
			name: 'y-velocity'
		};

		var z_velocity_trace = {
			x: time,
			y: payload_z_velocity,
			type: 'scatter',
			name: 'z-velocity'
		};

		var data_velocity_2d = [x_velocity_trace, y_velocity_trace, z_velocity_trace];

		var layout_velocity_graph = {
			xaxis: {
				title: 'Time (s)'
			},
			yaxis: {
				title: 'Velocity (m/s)'
			},
			margin: {
				t: 10
			}
		};

		Plotly.plot(Velocity_2D,
			data_velocity_2d,
			layout_velocity_graph	
		);

		//Acceleration graph
		var x_acceleration_trace = {
			x: half_times,
			y: payload_x_acceleration,
			type: 'scatter',
			name: 'x-acceleration'
		};

		var y_acceleration_trace = {
			x: half_times,
			y: payload_y_acceleration,
			type: 'scatter',
			name: 'y-acceleration'
		};

		var z_acceleration_trace = {
			x: half_times,
			y: payload_z_acceleration,
			type: 'scatter',
			name: 'z-acceleration'
		};

		var data_acceleraton_2d = [x_acceleration_trace, y_acceleration_trace, z_acceleration_trace];

		var layout_acceleration_graph = {
			xaxis: {
				title: 'Time (s)'
			},
			yaxis: {
				title: 'Acceleration (g)'
			},
			margin: {
				t: 10
			}
		};

		Plotly.plot(Acceleration_2D,
			data_acceleraton_2d,
			layout_acceleration_graph	
		);

	});
}

//Displays output on predicted trajectory page
function Print_Output() {
	const fs = require('fs');
	let rawdata = fs.readFileSync('output_values.json');
	console.log(rawdata);
	let json_trajectory_output = JSON.parse(rawdata);
	console.log(json_trajectory_output);

	document.getElementById('payload_total_flight_time_output').innerHTML = json_trajectory_output.payload_total_flight_time.toFixed(4);
	document.getElementById('payload_apogee_output').innerHTML = json_trajectory_output.payload_apogee.toFixed(4);
	document.getElementById('payload_time_of_apogee_output').innerHTML = json_trajectory_output.payload_time_of_apogee.toFixed(4);
	document.getElementById('payload_final_x_position_output').innerHTML = json_trajectory_output.payload_final_x_position.toFixed(4);
	document.getElementById('payload_final_y_position_output').innerHTML = json_trajectory_output.payload_final_y_position.toFixed(4);
	document.getElementById('payload_horizontal_distance_output').innerHTML = json_trajectory_output.payload_horizontal_distance.toFixed(4);
	document.getElementById('payload_landing_velocity_output').innerHTML = json_trajectory_output.payload_landing_velocity.toFixed(4);
	document.getElementById('payload_final_mass_output').innerHTML = json_trajectory_output.payload_final_mass.toFixed(4);

}

//Free memory used by electron
function Clear_Memory() {
	const {BrowserWindow} = require('electron');

	const win = BrowserWindow.getAllWindows()[0];
	const ses = win.webContents.session;

	ses.clearCache(() => {
		alert("Cache cleared!");
	});
}
