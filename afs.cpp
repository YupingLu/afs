/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560

This program implements a simple UNIX-like file system with a hierarchical directory 
structure. It support basic shell commands. These commands include mkfs, open, 
read, write, seek, close, mkdir, rmdir, cd, link, unlink, stat, ls, cat, cp, tree,
import, export, exit.*/


#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream> 
#include <string>
#include <cmath>
#include <time.h> 
#include "virtual_disk.cpp"
#include "afs.h"
#include "shell.h"
#include "shell/write.cpp"
#include "shell/cat.cpp"
#include "shell/cd.cpp"
#include "shell/link.cpp"
#include "shell/ls.cpp"
#include "shell/mkdir.cpp"
#include "shell/mkfs.cpp"
#include "shell/open.cpp"
#include "shell/rm.cpp"
#include "shell/rmdir.cpp"
#include "shell/space.cpp"
#include "shell/unlink.cpp"
#include "shell/close.cpp"
#include "shell/read.cpp"
#include "shell/seek.cpp"
#include "shell/stat.cpp"
#include "shell/cp.cpp"
#include "shell/tree.cpp"
#include "shell/import.cpp"
#include "shell/export.cpp"

using namespace std;


//core disk functions

// Initialize the disk. Block 0 and 1 are reserved.
int Initialize()
{
	int fd;		
	bool new_disk;	
	int i;		

	struct Bitmaps super_block;	// used to initialize block 0
	struct Dir_Inode dir_block;		// used to initialize block 1
	struct Data_Block data_block;	// used to initialize other blocks

	// Mount the disk
	new_disk = MountDrive(VIRTUAL_DISK, &fd);

	// Check for a new disk.  If we have a new disk, we must continue and
	// format the disk.
	if (!new_disk) return fd;

	// Initialize the superblock
	super_block.bitmap[0] = 0x3;		// mark blocks 0 and 1 as used
	for (i = 1; i < BLOCK_SIZE; i++) {
		super_block.bitmap[i] = 0;
	}

	// Write the superblock to block 0
	WriteDisk(fd, 0, (void *) &super_block);

	// Initialize the root directory
	dir_block.magic = DIR_MAGIC_NUM;
	dir_block.num_entries = 0;
	dir_block.parent_id = 1;   // root dir  is block 1
	dir_block.timer = 0;
	for (i = 0; i < MAX_FILES; i++) {
		dir_block.dir_entries[i].block_num = 0;
	}

	WriteDisk(fd, 1, (void *) &dir_block);

	for (i = 0; i < BLOCK_SIZE; i++) {
		data_block.data[i] = 0;
	}

	for (i = 2; i < NUM_BLOCKS; i++) {
		WriteDisk(fd, i, (void *) &data_block);
	}

	return fd;
}

// Gets a free block from the disk.
short AddNewBlock(int fd)
{
	struct Bitmaps super_block;	// super block - block 0
	int byte;	// byte of super block
	int bit;	// bit checked to see if block is available

	ReadDisk(fd, 0, (void *) &super_block);

	for (byte = 0; byte < BLOCK_SIZE; byte++) {

		// check to see if byte has available slot
		if (super_block.bitmap[byte] != 0xFF) {

			// loop to check each bit
			for (bit = 0; bit < 8; bit++) {
				int mask = 1 << bit;	// used to determine if bit is set or not
				if (mask & ~super_block.bitmap[byte]) {
					// Available block is found: set bit in bitmap, write result back
					// to superblock, and return block number.
					super_block.bitmap[byte] |= mask;
					WriteDisk(fd, 0, (void *) &super_block);
					return (byte * 8) + bit;
				}
			}
		}
	}

	// disk is full
	return -1;
}

// Free Block
void FreeBlock(int fd, short block_id)
{
	struct Bitmaps super_block;	// super block - block 0
	int byte = block_id / 8;		// byte number
	int bit = block_id % 8;		// bit number
	unsigned char mask = ~(1 << bit);	// mask to clear bit

	// get superblock
	ReadDisk(fd, 0, (void *) &super_block);

	// clear bit
	super_block.bitmap[byte] &= mask;

	// write back superblock
	WriteDisk(fd, 0, (void *) &super_block);
}

int main()
{
	int fd;			  // file descriptor for disk
	char cmd_string[MAX_CMD_NUM + 1]; // command line
	struct Cmd_Set command;		  // command struct
	bool status;			  // check for invalid command line
	int gfd;    //global current file descriptor
	int flag;	//file flag, initial value is 0, 1: r, 2: w, 3: rw
	int offset;		//file descriptor offset
	//FILE * pFile;

	Dir_Inode current_block;

	short current_dir = ROOT_DIR;		//set to root directory initially

#if 0
	cout << "superblock size: " << sizeof(struct Bitmaps) << endl;
	cout << "dirblock size: " << sizeof(struct Dir_Inode) << endl;
	cout << "inode size: " <<  sizeof(struct File_Inode) << endl;
	cout << "datablock size: " << sizeof(struct Data_Block) << endl;
#endif

	/*pFile = fopen ("samplescript.sh" , "r");
   	if (pFile == NULL)
   	{
   		perror ("Error opening file");
   		exit(-1);
   	}*/

	// Open the disk
	fd = Initialize();
	gfd = fd;
	flag = 0;
	offset = 0;

	while (1) {

		cout << START;
		//if (fgets(cmd_string, MAX_CMD_NUM, pFile) == NULL) continue;
		if (fgets(cmd_string, MAX_CMD_NUM, stdin) == NULL) continue;
		
		// Create the command structure, checking for invalid command lines
		status = PartitionCommand(cmd_string, command);
		if (!status) continue;

		ReadDisk(fd, current_dir, (void *) &current_block);
		// Look for the matching command
		if (strcmp(command.cmd_name, "mkdir") == 0) 
			mkdir(current_block, command, current_dir, fd);

		else if (strcmp(command.cmd_name, "ls") == 0) 
			ls(current_block, command, fd);
		
		else if (strcmp(command.cmd_name, "tree") == 0) {
			cout << "." <<endl; //current root
			tree(current_block, command, fd, 0);
		}
		
		else if (strcmp(command.cmd_name, "stat") == 0) 
			stat(current_block, command, fd);
		
		else if (strcmp(command.cmd_name, "cd") == 0) 
			cd(current_block, command, current_dir, fd);
		
		else if (strcmp(command.cmd_name, "home") == 0) {
			current_dir = ROOT_DIR;
			cout << "Home directory entered. " << endl;
		}
		
		else if (strcmp(command.cmd_name, "mkfs") == 0) {
			current_dir = ROOT_DIR;
			fd = mkfs();
			gfd = fd;
			flag = 0;
			offset = 0;
			cout << "Disk formated. " << endl;
		}
		
		else if (strcmp(command.cmd_name, "rmdir") == 0) 
			rmdir(current_block, command, current_dir, fd);
		
		else if (strcmp(command.cmd_name, "open") == 0) 
			open(current_block, command, current_dir, fd, gfd, flag);
		
		else if (strcmp(command.cmd_name, "link") == 0) 
			link(current_block, command, current_dir, fd);
		
		else if (strcmp(command.cmd_name, "unlink") == 0) 
			unlink(current_block, command, current_dir, fd);
		
		else if (strcmp(command.cmd_name, "cp") == 0) 
			cp(current_block, command, current_dir, fd);
		
		else if (strcmp(command.cmd_name, "import") == 0) 
			import(current_block, command, current_dir, fd);
		
		else if (strcmp(command.cmd_name, "export") == 0) 
			output(current_block, command, current_dir, fd);

		else if (strcmp(command.cmd_name, "write") == 0) 
			write(current_block, command, fd, gfd, flag);
			
		else if (strcmp(command.cmd_name, "read") == 0)
			read(command, fd, gfd, flag, offset);
		
		else if (strcmp(command.cmd_name, "seek") == 0)
			seek(command, fd, gfd, flag, offset);

		else if (strcmp(command.cmd_name, "cat") == 0) 
			cat(current_block, command, fd);

		else if (strcmp(command.cmd_name, "rm") == 0) 
			rm(current_block, command, current_dir, fd);
		
		else if (strcmp(command.cmd_name, "space") == 0) 
			space(fd);

		else if (strcmp(command.cmd_name, "exit") == 0) {
			UnmountDrive(fd);
			exit(0);
		}
		
		else if (strcmp(command.cmd_name, "close") == 0)
			close(command, fd, gfd, flag, offset);
		
		else {
			cerr << "ERROR: Invalid command not detected by PartitionCommand" << endl;
		}
	}
	//fclose (pFile);
	return 0;
}

//Receive the command string and divide it into command, file name and data
bool PartitionCommand(char *cmd_string, struct Cmd_Set &command)
{
	const char *DELIM  = " \t\n"; // delimiters
	int numtokens = 0;		// number of tokens
	const char *new_cmd;		// command string without leading spaces
	char *temp_string;		// temporary string used by strtok
	char *temp_cmd;
	int temp;

	// Remove spaces at beginning
	new_cmd = cmd_string + strspn(cmd_string, DELIM);
	if (strlen(new_cmd) == 0) {
		cerr << "Empty command line" << endl;
		return false;
	}

	// Create new string
	temp_string = (char *) new char[strlen(new_cmd) + 1];
	if (temp_string == NULL) {
		cerr << "new failed" << endl; 
		exit(-1);
	}
	strcpy(temp_string, new_cmd);

	// Count the number of tokens in the command */
	if (strtok(temp_string, DELIM) != NULL)
		for (numtokens = 1; strtok(NULL, DELIM) != NULL; numtokens++) ; 

	// Check for empty command line
	if (numtokens == 0) {
		cerr << "Empty command line" << endl;
		delete [] temp_string;
		return false;
	}

	// Extract the data
	strcpy(temp_string, new_cmd);
	command.cmd_name = strtok(temp_string, DELIM);
	if (numtokens > 1) command.file_name = strtok(NULL, DELIM);
	if (numtokens > 2) {
		temp_cmd = cmd_string + strspn(cmd_string, DELIM) + strlen(command.cmd_name)
		+ strlen(command.file_name) + 2*strlen(DELIM) - 4;
		command.data = temp_cmd;
		//printf ("%s\n",command.data);
	}

	// Check for invalid command lines
	if (strcmp(command.cmd_name, "ls") == 0 ||
		strcmp(command.cmd_name, "home") == 0 ||
		strcmp(command.cmd_name, "space") == 0 ||
		strcmp(command.cmd_name, "mkfs") == 0 ||
		strcmp(command.cmd_name, "exit") == 0 ||
		strcmp(command.cmd_name, "tree") == 0)
	{
		if (numtokens != 1) {
			cerr << "Invalid command line: " << command.cmd_name;
			cerr << " has improper number of arguments" << endl;
			return false;
		}
	}
	else if (strcmp(command.cmd_name, "mkdir") == 0 ||
		strcmp(command.cmd_name, "cd") == 0 ||
		strcmp(command.cmd_name, "rmdir") == 0 ||
		strcmp(command.cmd_name, "cat") == 0 ||
		strcmp(command.cmd_name, "rm") == 0 ||
		strcmp(command.cmd_name, "unlink") == 0 ||
		strcmp(command.cmd_name, "close") == 0 ||
		strcmp(command.cmd_name, "stat") == 0)
	{
		if (numtokens != 2) {
			cerr << "Invalid command line: " << command.cmd_name;
			cerr << " has improper number of arguments" << endl;
			return false;
		}
		if (strlen(command.file_name) >= MAX_FNAME_SIZE) {
			cerr << "Invalid command line: " << command.file_name;
			cerr << " is too long for a file name" << endl;
			return false;	
		}
	}
	else if (strcmp(command.cmd_name, "open") == 0 ||
		strcmp(command.cmd_name, "link") == 0 ||
		strcmp(command.cmd_name, "read") == 0 ||
		strcmp(command.cmd_name, "seek") == 0 ||
		strcmp(command.cmd_name, "cp") == 0 ||
		strcmp(command.cmd_name, "import") == 0 ||
		strcmp(command.cmd_name, "export") == 0)
	{
		if (numtokens != 3) {
			cerr << "Invalid command line: " << command.cmd_name;
			cerr << " has improper number of arguments" << endl;
			return false;
		}
		if (strlen(command.file_name) >= MAX_FNAME_SIZE) {
			cerr << "Invalid command line: " << command.file_name;
			cerr << " is too long for a file name" << endl;
			return false;
		}
	}
	else if (strcmp(command.cmd_name, "write") == 0)
	{
		if (numtokens < 3) {
			cerr << "Invalid command line: " << command.cmd_name;
			cerr << " has improper number of arguments" << endl;
			return false;
		}
		if (strlen(command.file_name) >= MAX_FNAME_SIZE) {
			cerr << "Invalid command line: " << command.file_name;
			cerr << " is too long for a file name" << endl;
			return false;
		}
	}
	else {
		cerr << "Invalid command line: " << command.cmd_name;
		cerr << " is not a command" << endl; 
		return false;
	} 

	return true;
}     

