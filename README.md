The project for infecting elf file and fork a key logger process.
ELFsrc  : the programs for infecting elf.
		include: 
			infector.c:  	The program to infect the first virus.
			fullnelson.c:	The program to get root.
			fullnelson.h:	The interface for get root.
			Tools:			Some tools are used in the project.
			virusBody:		The virus body what we want to insert to another elf.

getRoot : the programs for get root.
		include :
			fullnelson.c： 	
			fullnelson.h:

hideprocess: hide process by share library.
		processhider.c:  the main program for hide process.
		
ProcessHider_Modules: hide process by LKM.

skeylogger	: programs for logging the input by keyboard.

test  		: Some test file for infecting.

There are some path in the virus, you should change it before you use:

virusBody : the key logger file path.
skeylogger: the address for skeylogger.log and the target computer's IP.
 		