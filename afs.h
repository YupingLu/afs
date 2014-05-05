/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560

This program implements a simple UNIX-like file system with a hierarchical directory 
structure. It support basic shell commands. These commands include mkfs, open, 
read, write, seek, close, mkdir, rmdir, cd, link, unlink, stat, ls, cat, cp, tree,
import, export.*/

#ifndef AFS_H
#define AFS_H

// Global constants
const char *START = "AFS> ";
const char *VIRTUAL_DISK = "virtual_disk";
const unsigned int DIR_MAGIC_NUM = 0xFFFFFFFF;
const unsigned int INODE_MAGIC_NUM = 0xFFFFFFFE;
const int MAX_CMD_NUM = 500;
const int MAX_FNAME_SIZE = 200; //144;
const int MAX_BLOCKS = ((BLOCK_SIZE - 8) / 2) - 5;
const int MAX_FILES = 20; //28;
const int MAX_FILE_SIZE	= (MAX_BLOCKS * BLOCK_SIZE);
const int ROOT_DIR = 1;
const int FILE_BLOCK = 1;
const int BYTE_SIZE = 8;

// Commands
struct Cmd_Set
{
	char *cmd_name;	
	char *file_name;
	char *data;
};

bool PartitionCommand(char *cmd_string, struct Cmd_Set &command);

// Store the bitmap of free blocks
struct Bitmaps {
	unsigned char bitmap[BLOCK_SIZE]; 
};

// Directory Inode
struct Dir_Inode {
	unsigned int magic;		
	unsigned int num_entries;	
	
	int parent_id;
	int dump[9]; //to fix the size of 4096, no meaning here
	time_t timer;
	
	struct {
		char name[MAX_FNAME_SIZE];	
		short block_num;		
	} dir_entries[MAX_FILES];	
};

// File Inode
struct File_Inode {
	unsigned int magic;		
	unsigned int size;
	time_t timer;
	short blocks[MAX_BLOCKS];	
	short link_count;	
};

// Data region
struct Data_Block {
	char data[BLOCK_SIZE];
};

// Apply new blocks from the disk
short AddNewBlock(int fd);
int Initialize();

// Free Block.
void FreeBlock(int fd, short block_id);

#endif
