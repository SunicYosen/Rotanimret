# ROTANIMERT

The project for infecting elf file and fork a key logger process.

## ELFsrc

The programs for infecting elf.

1. infector.c: The program to infect the first virus.
2. fullnelson.c: The program to get root.
3. fullnelson.h: The interface for get root.
4. Tools: Some tools are used in the project.
5. virusBody: The virus body what we want to insert to another elf.

## getRoot

The programs for get root.

1. fullnelson.c
2. fullnelson.h

## hideprocess

hide process by share library.

1. processhider.c:  the main program for hide process.

2. ProcessHider_Modules: hide process by LKM.

3. skeylogger: programs for logging the input by keyboard.

## test

Some test file for infecting.

Attation: There are some path in the virus, you should change it before you use:

## virusBody

The key logger file path.

1. skeylogger: the address for skeylogger.log and the target computer's IP.