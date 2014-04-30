/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */

// This implements shell commands.

#ifndef SHELL_H
#define SHELL_H

// Command processing
//intializes a new directory
dirblock_t mkdir();	
				
//take a new initalized directory and place it in the current subdir
void makeDir(dirblock_t curBlock, cmd_t command, int curDir, int disk);	

//output the current subdirectory
void ls(dirblock_t curBlock, cmd_t command, int disk);

//return whether or not the blockNum is a dir or not
bool isDir(short blockNum, int disk);

//push the user into an existing subdirectory
void cd(dirblock_t curBlock, cmd_t command, short &curDir, int disk);

//remove a current existing directory(or not)
void rmDir(dirblock_t curBlock, cmd_t command, short curDir, int disk);

//delete all the associated datablocks with the file
dirblock_t deleteAll(short blockNum, int disk);

//create a file with iNode implementation
void createF(dirblock_t curBlock, cmd_t command, short curDir, int disk);

//append to the current file
void append(dirblock_t curBlock, cmd_t command, int disk);

//output the iNode 
void cat(dirblock_t curBlock, cmd_t command, int disk);

//remove the iNode file given
void rm(dirblock_t curBlock, cmd_t command, short curDir, int disk);

//output the current space of the disk
void space(int disk);

//output the current taken blocks of the disk
int getTaken(int disk);

//initialize a iNode & returns it
inode_t create();

//format the disk
int mkfs();

//Create a link named <dest> to an existing file named <src>
void link(dirblock_t curBlock, cmd_t command, short curDir, int disk);

//Remove a link to the name. (When the link count drops to 0, delete the file)
void unlink(dirblock_t curBlock, cmd_t command, short curDir, int disk);
#endif
