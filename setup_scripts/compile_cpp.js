// Used for compiling the trajectory calculator executable

function print_end(){
  number_of_compiler_process += -1;
  if (number_of_compiler_process == 0){
    console.log("\n###################################");
  }
}

const fs = require("fs"); // Used for checking if exeutable directing exists
if (fs.existsSync("executables")) {
	// Do nothing
} else {
	console.log("Directory 'executables' not found...\nCreating 'executables' directory");
	fs.mkdirSync("executables");
}

const { exec } = require('child_process');

var number_of_compiler_process = 2;
console.log("###################################");
console.log("Compiling Necessary C++ Files...\n");

const { spawn } = require('child_process');
const child = spawn('g++ RAA_Trajectory/trajectory_driver.cpp -o executables/trajectory_driver', {
  stdio: 'inherit',
  shell: true
});

//Listener function that reports back when child process has closed
child.on('exit', function(code, signal) {
  console.log("RAA Trajectory compiled into trajectory_driver located in executables");
  print_end();
});

const child_2 = spawn('g++ RAA_Trajectory/raa_trajectory_cli.cpp -o executables/raa_trajectory_cli', {
	stdio: 'inherit',
	shell: true
});

child_2.on('exit', function(code, signal) {
  console.log("RAA Trajectory CLI compiled into raa_trajectory_cli located in executables");
  print_end();
});
