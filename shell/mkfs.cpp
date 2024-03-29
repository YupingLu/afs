/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//Make a new file system, i.e., format the disk so that it is
//ready for other file system operations.

using namespace std;


int mkfs()
{
	int fd;		// file descriptor for disk
	bool new_disk;	// set if new disk was created
	int i;		// loop traversal variable

	struct Bitmaps super_block;	// used to initialize block 0
	struct Dir_Inode dir_block;		// used to initialize block 1
	struct Data_Block data_block;	// used to initialize other blocks

	// Mount the disk
	new_disk = MountDrive(VIRTUAL_DISK, &fd);

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

	// Write the root directory to block 1
	WriteDisk(fd, 1, (void *) &dir_block);

	// Initialize the data block to all zeroes
	for (i = 0; i < BLOCK_SIZE; i++) {
		data_block.data[i] = 0;
	}

	// Write the data block to all other blocks on disk
	for (i = 2; i < NUM_BLOCKS; i++) {
		WriteDisk(fd, i, (void *) &data_block);
	}

	return fd;
}

