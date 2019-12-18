// This scripts deletes files made from the 'npm run first_install' command.


var fs = require('fs-extra');

var files_to_delete = [
	"executables",
	"node_modules",
	"output_data.csv",
	"output_values.json",
	"package-lock.json"
];

for (var i = 0; i < files_to_delete.length; i++) {
	fs.remove(files_to_delete[i], err => {
		if (err) {
			console.log("Does not exist. Moving to next file");
		}
	});
}


