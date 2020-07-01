#!/usr/bin/expect -f
# Script to (insecurly) login into darwin and move files then compile them
set timeout -1

spawn scp motorControl.cpp ColorOutput.h ColorOutput.cpp Makefile darwin@192.168.1.9:/darwin/Linux/project/motor_Control

expect "*password: "

send -- "111111\r"

expect eof

spawn ssh darwin@192.168.1.9

expect "*password: "

send -- "111111\r"

expect "*$ "

send -- "cd /darwin/Linux/project/motor_Control\r"

expect -- "*$ "

send -- "make\r"

expect "*$ "

interact
