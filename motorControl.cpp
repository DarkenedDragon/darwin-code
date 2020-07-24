/**
* @author Eli Jelesko 
*/


#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include "LinuxDARwIn.h"
#include "ColorOutput.h"

using namespace std;
using namespace Robot;

// If reassining ID's is added this setup will need to change
const unsigned int NUM_MOTORS = 20;
const string jointNames[] = {"Unassigned", "R SHOULDER PITCH", "L SHOULDER PITCH", "R SHOULDER ROLL", "L SHOULDER ROLL", "R ELBOW", "L ELBOW", "R HIP YAW", "L HIP YAW", "R HIP ROLL", "L HIP ROLL", "R HIP PITCH", "L HIP PITCH", "R KNEE", "L KNEE", "R ANKLE PITCH", "L ANKLE PITCH", "R ANKLE ROLL", "L ANKLE ROLL", "HEAD PAN", "HEAD TILT"}; // ID should correspond to place in array
// Checks if a particular ID is up
bool ping(CM730 &cm730, unsigned int id, bool showOutput) {
	bool result = cm730.Ping(id, 0) == CM730::SUCCESS;
	if (showOutput) {
		string message = result ? ColorOutput::colorize("[$bg{Connected}]") : ColorOutput::colorize("[$br{Disconnected}]");  // assigns connected if result is true
		cout << left;
		if (id >= 0 && id <= NUM_MOTORS) {
			cout << setw(20) << jointNames[id] << setw(20) << id << setw(20) << message << endl;
		} else {
			cout << setw(20) << jointNames[0] << setw(20) << id << setw(20) << message << endl;
		}
	}
	return result;
}

bool isNum(string& str) {
	return str.find_first_not_of("0123456789") == std::string::npos;
}

// CAN OVERFLOW!!!
int stoi(string& str) {
	int ret = 0;
	// value - 48
	for (unsigned int i = 0; i < str.size(); i++) {
		ret += ((int)str.at(i) - 48) * pow(10, str.size() - i - 1);	
	}

	return ret;
}

/**
* Read in a command line
*/
void readLine(vector<string>& input) {
	input.clear(); // clean out previous

	string line;
	string val;

	cout << "control> ";
	std::getline(cin, line); // read in the command
	stringstream ss(line);
	while (ss >> val ) { // put all the space seperated values into the vector
		input.push_back(val);
	}
}


int main() {

	LinuxCM730 lxCM730("/dev/ttyUSB0");
	CM730 cm730(&lxCM730); // the subcontroller

	// Connect to the board
	cout << "Connecting to subcontroller..." << endl;
	if (!cm730.Connect()) {
		cout << "Could not connect to the subcontroller. Please wait and try again." << endl;
		//return 0;

		// debugging 
		// Need to beable to check if the subcontroller has a new/bad ID
		cout << endl << "DEBUG" << endl;
		cout << "Please disconnect all motors, then press ENTER. If you don't wish to continue debugging, simply exit the program" << endl;
		cin.get();
		cout << "Attempting to locate subcontroller" << endl;
		for (int i = 0; i <= 255; i++) {
			if (cm730.WriteByte(i, CM730::P_DXL_POWER, 1, 0) == CM730::SUCCESS) {
				cout << "Sucessfully powered on device at id " << i << endl;
				cout << "Reassign this device to ID 200? (This is presumably the subcontroller) [y/n]" << endl;
				string response;
				cin >> response;
				if (response == "y" || response == "Y" || response == "yes" || response == "YES") {
					int result = cm730.WriteByte(i, MX28::P_ID, 200, 0); // id, address, value, error
					if (result != 0) { // something went wrong..
						cout << "Error writing to ID " << i << endl;
					} else {
						cout << "Successfully reassigned " << i << " to " << 200 << endl;
						break;
					}

				}
			}
		}
		
		cout << "The program will now exit" << endl;
		return 0;
	}

	vector<string> input;
	while(true) {
		readLine(input); // get commands

		// determine which command has been entered
		string command = input.at(0);

		if (command == "ping" && input.size() == 2) { // checks that it is a ping with one parameter
			if (input.at(1) == "all") { // all motors commanded
				cout << left; // left justified
				cout << setw(20) << "Joint" << setw(20) << "ID" << setw(20) << "Status" << endl;
				for (unsigned int i = 0; i <= NUM_MOTORS; i++) {
					ping(cm730, i, true); 
				}
				cout << endl;

			} else if (isNum(input.at(1))) { // check that the parameter is a number
				ping(cm730, stoi(input.at(1)), true);	
			} else {
				cout << "Invalid Command" << endl;
			}

		} else if (command == "check" && input.size() == 1) {
			cout << "Checking for connected motors" << endl;	
			for (int i = 0; i < 255; i++) {
				bool result = ping(cm730, i, false);
				cout << left;
				if (result) {
					string message = result ? ColorOutput::colorize("[$bg{Connected}]") : ColorOutput::colorize("[$br{Disconnected}]");  // assigns connected if result is true

					if (i >= 0 && i <= NUM_MOTORS) {
						cout << setw(20) << jointNames[i] << setw(20) << i << setw(20) << message << endl;
					} else if (i == 200) {
						cout << setw(20) << "SUB-CONTROLLER" << setw(20) << i << setw(20) << message << endl;
					} else if (i == 254) {
						cout << setw(20) << "ID BROADCAST" << setw(20) << i << setw(20) << message << endl;
					} else {
						cout << setw(20) << "Unknown" << setw(20) << i << setw(20) << message << endl;
					}
				}
			}
		} else if (command == "assign" && input.size() == 3) {
			// assign [CURRENT ID] [NEW ID]
			if (isNum(input.at(1)) && isNum(input.at(2))) {
				int currentID = stoi(input.at(1));
				int newID = stoi(input.at(2));
				bool currentUp = ping(cm730, currentID, false);
				bool newUp = ping(cm730, newID, false);

				// Check that it isn't the board's id
				if (currentID == 200) {
					cout << "Cannot reassign subcontroller ID" << endl;
					continue;
				} 
				// Check that this can be a vaild ID
				if (newID < 0 || newID > 252) {
					cout << "Cannot assign to that ID. Please use an ID in the range 0 - 252." << endl;
					continue;
				}
				// Check that the new id is available
				if (newUp) {
					cout << "New ID is already in use" << endl;
					continue;
				}
				// Check that the current id is connected
				if (currentUp) {
					int result = cm730.WriteByte(currentID, MX28::P_ID, newID, 0); // id, address, value, error
						if (result != 0) { // something went wrong..
							cout << "Error writing to ID " << currentID << endl;
						} else {
							cout << "Successfully reassigned " << currentID << " to " << newID << endl;
						}

				} else {
					cout << "ID is not connected" << endl;
					continue;
				}
			} else {
				cout << "ID's must be a positive number" << endl;
			}
		} else if (command == "help") {
			// set spacing
			unsigned int first = 20;
			unsigned int second = 30;
			unsigned int third = 20;
			cout << left << endl;
			cout << setw(first) << "[Command]" << setw(second) << "[Parameters]" << setw(third) << "[Description]" << endl << endl;
			cout << setw(first) << "help" << setw(second) << "None" << setw(third) << "Displays this help screen" << endl;
			cout << setw(first) << "ping" << setw(second) << "[ID]" << setw(third) << "Pings the selected motor. Using all as an ID will command all motors" << endl;
			cout << setw(first) << "led" << setw(second) << "[ID] [on/off]" << setw(third) << "Turns the selected motor's LED on or off. Using all as an ID will command all motors" << endl;
			cout << setw(first) << "pos" << setw(second) << "[ID] [GOAL_POSITION]" << setw(third) << "Commands the selected motor to move to GOAL_POSITION" << endl;
			cout << setw(first) << "cpos" << setw(second) << "[ID]" << setw(third) << "Returns the current position of the selected motor" << endl;
			cout << setw(first) << "mpos" << setw(second) << "[ID]" << setw(third) << "Frees the motor and continuously displays its current position." << endl;


			cout << endl << endl << endl;


		} else if (command == "led" && input.size() == 3) {
			// Make sure the last arguements are correct	
			if (input.at(2) == "on" || input.at(2) == "ON" || input.at(2) == "off" || input.at(2) == "OFF") {
				// determine the value to send to writeByte
				int val = 0;
				if (input.at(2) == "on" || input.at(2) == "ON") {
					val = 1;
				}

				if (input.at(1) == "all") { // write to all available motors
					for (unsigned int i = 1; i <= NUM_MOTORS; i++) {
						if (ping(cm730, i, false)) { // see if we can actually reach the motor
							int result = cm730.WriteByte(i, MX28::P_LED, val, 0); // id, address, value, error
							if (result != 0) { // something went wrong..
								cout << "Error writing to ID " << i << endl;
							}
						}
					}
				} else if (isNum(input.at(1))) { // write to an individual motor
					int id = stoi(input.at(1));
					if (ping(cm730, id, false)) { // see if we can actuall reach the motor
						int result = cm730.WriteByte(id, MX28::P_LED, val, 0); // id, address, value, error
						if (result != 0) { // something went wrong..
							cout << "Error writing to ID " << id << endl;
						}
					}

				} else {
					cout << "Invalid Command" << endl;
				}
			} else {
				cout << "Invalid Command" << endl;
			}
		} else if (command == "pos" && input.size() == 3) { // To move the motor to a specified angle
			// Confirm the parameters make sense
			if (isNum(input.at(1)) && isNum(input.at(2))) { // both parameters are good
				// pretty sure all result codes are positive
				int res = 0;
				int id = stoi(input.at(1));
				int val = stoi(input.at(2));
				int error = 0;
				// set PID values
				res += cm730.WriteByte(id, MX28::P_P_GAIN, 32, &error);
				res += cm730.WriteByte(id, MX28::P_I_GAIN, 0, &error);
				res += cm730.WriteByte(id, MX28::P_D_GAIN, 0, &error);

				if (res != CM730::SUCCESS || error != 0) {
					cout << "Error writing PID gains" << endl;
				}

				// write the actual goal position
				// TODO: Make sure this can't overextend the motor!!!
				res += cm730.WriteWord(id, MX28::P_GOAL_POSITION_L, val, &error);

				if (res != CM730::SUCCESS || error != 0) {
					cout << "Error writing position to ID  " << id << endl;
				}

			} else {
				cout << "Invalid Command" << endl;
			}
		} else if (command == "cpos" && input.size() == 2) {
			
			if (isNum(input.at(1))) {
				int res = 0;
				int retVal = 0;
				int error = 0;
				int id = stoi(input.at(1));

				res = cm730.ReadWord(id, MX28::P_PRESENT_POSITION_L, &retVal, &error);
				
				if (res != CM730::SUCCESS || error != 0) {
					cout << "Error reading position from ID  " << id << endl;
				} else {
					cout << "pos: " << retVal << endl;
				}

			} else {
				cout << "Invalid Commmand" << endl;
			}
		} else if (command == "mpos" && input.size() == 2) {
			int retVal = 0;
			
			if (isNum(input.at(1)) && ping(cm730, stoi(input.at(1)), false)) {
				// disable torque
				int id = stoi(input.at(1));
				int torqRes = cm730.WriteByte(id, MX28::P_TORQUE_ENABLE, 0, 0);

				cout << "Use Ctrl-C to escape. Sorry :(" << endl;
				while (torqRes == 0) {	
					int res = 0;
					int error = 0;

					res = cm730.ReadWord(id, MX28::P_PRESENT_POSITION_L, &retVal, &error);
						
					if (res != CM730::SUCCESS || error != 0) {
						cout << "Error reading position from ID  " << id << endl;
						break;
					} else {
						string padding = "";
						if (retVal < 10) {
							padding = "000";
						} else if (retVal < 100) {
							padding = "00";
						} else if (retVal < 1000) {
							padding = "0";
						}
						// prints on top of itself
						cout << "\rpos: [" << padding << retVal << "]";
					}
				}
				cout << "Error disengaging the motor" << endl;
			} else {
				cout << "Invalid Commmand" << endl;
			}

		
		} else if (command == "exit") {
			return 0;

		} else {
			cout << "Invalid command" << endl;
		}

	}

	return 0;

}
