# darwin-code
Code for simple actions on a ROBOTIS DARWIN robot

The main code is in the motorControl.cpp file. 

ColorOutput contains a function to parse a string such that it is displays with a color. This uses ANSI escape sequences and thus may not work on all terminals. 

Use the send_it.sh file to quickly upload all the nessisary files to DARWIN, compile them, and then do what you will. You may need to adjust the IP address to 
suite your individual needs. 

## Supports
* Checking if a motor is connected
* Turning the LED's on/off
* Real time position monitoring
* One line motor movement command
