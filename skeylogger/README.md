This is the simple instruction for the keylogger.
It inculdes:
keylogger.c ---main function
options.c/h ---support some option for users when starting the whole program
config.c/h  ---basic configuration of the program including the address of logfile and deviceFile.
key_util.c/h ==map for real keyborad charcters and basic function, such as getNext character.
fullnelson.c/h-module for getting root based on fullnelson
init.sh     ---shell cript to install necessary packages like expect and generate the pair of public and private key and send it to the target machine.
Makefile    ---complie all the program in this module
The reference code is from the Github and the link is https://github.com/gsingh93/simple-key-logger

In order to run, you may first make and run the skeylogger, then the every keybaord event will be logged in the logfile.
