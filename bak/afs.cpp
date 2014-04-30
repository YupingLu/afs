/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560

This program implements a simple UNIX-like file system with a hierarchical directory 
structure. It support basic shell commands. These commands include mkfs, open, 
read, write, seek, close, mkdir, rmdir, cd, link, unlink, stat, ls, cat, cp, tree,
import, export, exit.*/

//Now support mkfs, cd, rmdir, rm, create, mkdir, space, ls, append, and cat. 

#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream> 
#include <string>
#include <cmath>
#include "disk.cpp"
#include "afs.h"
#include "shell.cpp"

using namespace std;


//core disk functions

// Opens the simulated disk file. If a disk file is created, this
// routines also "formats" the disk by initializing special blocks
// 0 (superblock) and 1 (root directory). */
int open_disk()
{
	int fd;		// file descriptor for disk
	bool new_disk;	// set if new disk was created
	int i;		// loop traversal variable

	struct superblock_t super_block;	// used to initialize block 0
	struct dirblock_t dir_block;		// used to initialize block 1
	struct datablock_t data_block;	// used to initialize other blocks

	// Mount the disk
	new_disk = mount_disk(DISK_NAME, &fd);

	// Check for a new disk.  If we have a new disk, we must continue and
	// format the disk.
	if (!new_disk) return fd;

	// Initialize the superblock
	super_block.bitmap[0] = 0x3;		// mark blocks 0 and 1 as used
	for (i = 1; i < BLOCK_SIZE; i++) {
		super_block.bitmap[i] = 0;
	}

	// Write the superblock to block 0
	write_disk_block(fd, 0, (void *) &super_block);

	// Initialize the root directory
	dir_block.magic = DIR_MAGIC_NUM;
	dir_block.num_entries = 0;
	for (i = 0; i < MAX_FILES; i++) {
		dir_block.dir_entries[i].block_num = 0;
		//dir_block.dir_entries[i].parent_num = 1;
	}

	// Write the root directory to block 1
	write_disk_block(fd, 1, (void *) &dir_block);

	// Initialize the data block to all zeroes
	for (i = 0; i < BLOCK_SIZE; i++) {
		data_block.data[i] = 0;
	}

	// Write the data block to all other blocks on disk
	for (i = 2; i < NUM_BLOCKS; i++) {
		write_disk_block(fd, i, (void *) &data_block);
	}

	return fd;
}

// Gets a free block from the disk.
short get_free_block(int disk)
{
	struct superblock_t super_block;	// super block - block 0
	int byte;	// byte of super block
	int bit;	// bit checked to see if block is available

	// get superblock
	read_disk_block(disk, 0, (void *) &super_block);

	// look for first available block
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
					write_disk_block(disk, 0, (void *) &super_block);
					return (byte * 8) + bit;
				}
			}
		}
	}

	// disk is full
	return 0;
}

// Reclaims block making it available for future use.
void reclaim_block(int disk, short block_num)
{
	struct superblock_t super_block;	// super block - block 0
	int byte = block_num / 8;		// byte number
	int bit = block_num % 8;		// bit number
	unsigned char mask = ~(1 << bit);	// mask to clear bit

	// get superblock
	read_disk_block(disk, 0, (void *) &super_block);

	// clear bit
	super_block.bitmap[byte] &= mask;

	// write back superblock
	write_disk_block(disk, 0, (void *) &super_block);
}

int main()
{
	int disk;			  // file descriptor for disk
	char cmd_str[MAX_CMD_NUM + 1]; // command line
	struct cmd_t command;		  // command struct
	bool status;			  // check for invalid command line
	dirblock_t newBlock;	
	dirblock_t curBlock;

	inode_t tempFile;

	short newBlockNum;
	short curDir = ROOT_DIR;		//set to root directory initially
	// Uncomment this section to make sure the size of the blocks are
	// equal to the block size.
//#if 0
	cout << "superblock size: " << sizeof(struct superblock_t) << endl;
	cout << "dirblock size: " << sizeof(struct dirblock_t) << endl;
	cout << "inode size: " <<  sizeof(struct inode_t) << endl;
	cout << "datablock size: " << sizeof(struct datablock_t) << endl;
//#endif

	// Open the disk
	disk = open_disk();

	while (1) {

		// Print prompt and get command line
		cout << PROMPT_STRING;
		if (fgets(cmd_str, MAX_CMD_NUM, stdin) == NULL) continue;

		// Create the command structure, checking for invalid command lines
		status = make_cmd(cmd_str, command);
		if (!status) continue;

		read_disk_block(disk, curDir, (void *) &curBlock);
		// Look for the matching command
		if (strcmp(command.cmd_name, "mkdir") == 0) 
			makeDir(curBlock, command, curDir, disk);

		else if (strcmp(command.cmd_name, "ls") == 0) 
			ls(curBlock, command, disk);
		
		else if (strcmp(command.cmd_name, "cd") == 0) 
			cd(curBlock, command, curDir, disk);
		
		else if (strcmp(command.cmd_name, "home") == 0) {
			curDir = ROOT_DIR;
			cout << "Home directory entered. " << endl;
		}
		
		else if (strcmp(command.cmd_name, "mkfs") == 0) {
			curDir = ROOT_DIR;
			disk = mkfs();
			cout << "Disk formated. " << endl;
		}
		
		else if (strcmp(command.cmd_name, "rmdir") == 0) 
			rmDir(curBlock, command, curDir, disk);
		
		else if (strcmp(command.cmd_name, "create") == 0) 
			createF(curBlock, command, curDir, disk);
		
		else if (strcmp(command.cmd_name, "link") == 0) 
			link(curBlock, command, curDir, disk);
		
		else if (strcmp(command.cmd_name, "unlink") == 0) 
			unlink(curBlock, command, curDir, disk);

		else if (strcmp(command.cmd_name, "append") == 0) 
			append(curBlock, command, disk);

		else if (strcmp(command.cmd_name, "cat") == 0) 
			cat(curBlock, command, disk);

		else if (strcmp(command.cmd_name, "rm") == 0) 
			rm(curBlock, command, curDir, disk);
		
		else if (strcmp(command.cmd_name, "space") == 0) 
			space(disk);

		else if (strcmp(command.cmd_name, "exit") == 0) {
			unmount_disk(disk);
			exit(0);
		}
		else {
			cerr << "ERROR: Invalid command not detected by make_cmd" << endl;
		}
	}

	return 0;
}

bool make_cmd(char *cmd_str, struct cmd_t &command)
{
	const char *DELIM  = " \t\n"; // delimiters
	int numtokens = 0;		// number of tokens
	const char *snew;		// command string without leading spaces
	char *temp_str;		// temporary string used by strtok

	// Remove spaces at beginning
	snew = cmd_str + strspn(cmd_str, DELIM);
	if (strlen(snew) == 0) {
		cerr << "Empty command line" << endl;
		return false;
	}

	// Create new string
	temp_str = (char *) new char[strlen(snew) + 1];
	if (temp_str == NULL) {
		cerr << "new failed" << endl; 
		exit(-1);
	}
	strcpy(temp_str, snew);

	// Count the number of tokens in the command */
	if (strtok(temp_str, DELIM) != NULL)
		for (numtokens = 1; strtok(NULL, DELIM) != NULL; numtokens++) ; 

	// Check for empty command line
	if (numtokens == 0) {
		cerr << "Empty command line" << endl;
		delete [] temp_str;
		return false;
	}

	// Extract the data
	strcpy(temp_str, snew);
	command.cmd_name = strtok(temp_str, DELIM);
	if (numtokens > 1) command.file_name = strtok(NULL, DELIM);
	if (numtokens > 2) command.data = strtok(NULL, DELIM);

	// Check for invalid command lines
	if (strcmp(command.cmd_name, "ls") == 0 ||
		strcmp(command.cmd_name, "home") == 0 ||
		strcmp(command.cmd_name, "space") == 0 ||
		strcmp(command.cmd_name, "mkfs") == 0 ||
		strcmp(command.cmd_name, "exit") == 0)
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
		strcmp(command.cmd_name, "create") == 0||
		strcmp(command.cmd_name, "cat") == 0 ||
		strcmp(command.cmd_name, "rm") == 0 ||
		strcmp(command.cmd_name, "unlink") == 0)
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
	else if (strcmp(command.cmd_name, "append") == 0)
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
	else if (strcmp(command.cmd_name, "link") == 0)
	{
		if (numtokens != 3) {
			cerr << "Invalid command line: " << command.cmd_name;
			cerr << " has improper number of arguments" << endl;
			return false;
		}
		if (strlen(command.data) >= MAX_FNAME_SIZE) {
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

