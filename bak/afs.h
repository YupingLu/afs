/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560

This program implements a simple UNIX-like file system with a hierarchical directory 
structure. It support basic shell commands. These commands include mkfs, open, 
read, write, seek, close, mkdir, rmdir, cd, link, unlink, stat, ls, cat, cp, tree,
import, export.*/

#ifndef AFS_H
#define AFS_H
struct cmd_t
{
	char *cmd_name;	// name of command
	char *file_name;	// name of file
	char *data;		// data (append only)
};
bool make_cmd(char *cmd_str, struct cmd_t &command);

// Constants

const char *PROMPT_STRING = "AFS> ";
const char *DISK_NAME = "DISK";
const unsigned int DIR_MAGIC_NUM = 0xFFFFFFFF;
const unsigned int INODE_MAGIC_NUM = 0xFFFFFFFE;
const int MAX_CMD_NUM = 500;
const int MAX_FNAME_SIZE = 144;
const int MAX_BLOCKS = ((BLOCK_SIZE - 8) / 2) - 1;
const int MAX_FILES = 28;
const int MAX_FILE_SIZE	= (MAX_BLOCKS * BLOCK_SIZE);
const int ROOT_DIR = 1;
const int FILE_BLOCK = 1;
const int BYTE_SIZE = 8;

// Block types

struct superblock_t {
	unsigned char bitmap[BLOCK_SIZE]; // bitmap of free blocks
};

struct dirblock_t {
	unsigned int magic;		// magic number, must be DIR_MAGIC_NUM
	unsigned int num_entries;	// number of files in directory
	//short parent_num;		//parent block number
	struct {
		char name[MAX_FNAME_SIZE];	// file name
		short block_num;		// block number of file (0 - unused)
		//short parent_num;		//set parent block number here, easy to divide block
	} dir_entries[MAX_FILES];	// list of directory entries
};

struct inode_t {
	unsigned int magic;		// magic number, must be INODE_MAGIC_NUM
	unsigned int size;		// file size in bytes
	short blocks[MAX_BLOCKS];	// array of direct indices to data blocks
	short link_count;	//count the link
};

struct datablock_t {
	char data[BLOCK_SIZE];	// data (BLOCK_SIZE bytes)
};

// Gets a free block from the disk.
short get_free_block(int disk);
int open_disk();
// Reclaims block making it available for future use.
void reclaim_block(int disk, short block_num);

#endif
