/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */

// This implements shell commands.

//#include "filesys.h"
#include "shell.h"

using namespace std;


//return whether a block is a directory
bool isDir(short blockNum, int disk)
{
	dirblock_t tempBlock;
	read_disk_block(disk, blockNum, (void*) &tempBlock);

	if(tempBlock.magic == DIR_MAGIC_NUM)
		return true;
	else
		return false;
}

//Get sub directory
dirblock_t subDir(short blockNum, int disk)
{
	dirblock_t tempBlock;
	read_disk_block(disk, blockNum, (void*) &tempBlock);

	return tempBlock;
}

//intialize a directory and return it
dirblock_t mkdir(){
	dirblock_t tempDir;
	tempDir.magic = DIR_MAGIC_NUM;
	tempDir.num_entries = 0;

	return tempDir;
}

//initialize a iNode
inode_t create(){
	inode_t tempINode;
	tempINode.magic = INODE_MAGIC_NUM;
	tempINode.size = 0;
	tempINode.link_count = 1;
	for(int i = 0; i < MAX_BLOCKS; i++)
		tempINode.blocks[i] = 0;

	return tempINode;
}

//Get a iNode
inode_t getiNode(short blockNum, int disk){
	inode_t tempINode;
	read_disk_block(disk, blockNum, (void*) &tempINode);

	return tempINode;
}

//create a directory
void makeDir(dirblock_t curBlock, cmd_t command, int curDir, int disk){
	const char *DELIM  = "/"; // delimiters
	int numtokens = 0;		// number of tokens
	char *temp_str;		// temporary string used by strtok
	char *dir1;		// sub directory of current directory
	char *dir2;		// directory in sub directory
	
	// Create new string
	temp_str = (char *) new char[strlen(command.file_name) + 1];
	if (temp_str == NULL) {
		cerr << "new failed" << endl; 
		exit(-1);
	}
	strcpy(temp_str, command.file_name);
	
	// Count the number of tokens in the command */
	if (strtok(temp_str, DELIM) != NULL)
		for (numtokens = 1; strtok(NULL, DELIM) != NULL; numtokens++) ; 
	
	// Extract the data
	strcpy(temp_str, command.file_name);
	dir1 = strtok(temp_str, DELIM);
	if (numtokens > 1) dir2 = strtok(NULL, DELIM);
	if (numtokens == 1) {
		bool there = false;
		bool isSpace = false;
		short newBlockNum;
		dirblock_t newBlock;


		for(int i = 0; i < MAX_FILES; i++)
		{
			if(strcmp(command.file_name, curBlock.dir_entries[i].name) == 0 && isDir(curBlock.dir_entries[i].block_num, disk))
				there = true;
			if(curBlock.dir_entries[i].block_num == 0)
				isSpace = true;
		}

		if(getTaken(disk) >= 32768)  //1024
			isSpace = false;

		if(!there && isSpace){
			newBlock = mkdir();
			newBlockNum = get_free_block(disk);
			write_disk_block(disk, newBlockNum, (void *) &newBlock);

			strcpy(curBlock.dir_entries[curBlock.num_entries].name, command.file_name);
			curBlock.dir_entries[curBlock.num_entries].block_num = newBlockNum;
			curBlock.num_entries++;
			write_disk_block(disk, curDir, (void *) &curBlock);
			cout << "Directory " << command.file_name << " is created." << endl; 
		}
		else if(!there && !isSpace)
			cout << "There is no space for the directory to be created in the current directory. " << endl;
		else
			cout << "Directory " << command.file_name << " is already created." << endl; 
	}
	else if (numtokens > 1) {
		bool there = false;
		bool isSpace = false;
		bool dir_exist = false;
		short newBlockNum;
		dirblock_t subBlock;
		dirblock_t newBlock;
		int tempDir;
		

		for(int i = 0; i < MAX_FILES; i++)
		{
			if(strcmp(dir1, curBlock.dir_entries[i].name) == 0 && isDir(curBlock.dir_entries[i].block_num, disk)) 
			{
				dir_exist = true;
				subBlock = subDir(curBlock.dir_entries[i].block_num, disk);
				tempDir = curBlock.dir_entries[i].block_num;
				
				for(int i = 0; i < MAX_FILES; i++)
				{
					if(strcmp(dir2, subBlock.dir_entries[i].name) == 0 && isDir(subBlock.dir_entries[i].block_num, disk))
						there = true;
					if(subBlock.dir_entries[i].block_num == 0)
						isSpace = true;
				}
			}
		
			
		}

		if(getTaken(disk) >= 32768)  //1024
			isSpace = false;

		if(!dir_exist){
			cout << "Directory " << dir1 << " does not exist." << endl; 
		}
		else if(!there && isSpace){
			newBlock = mkdir();
			newBlockNum = get_free_block(disk);
			write_disk_block(disk, newBlockNum, (void *) &newBlock);
			
			curDir = tempDir;
			
			strcpy(subBlock.dir_entries[subBlock.num_entries].name, dir2);
			subBlock.dir_entries[subBlock.num_entries].block_num = newBlockNum;
			subBlock.num_entries++;
			write_disk_block(disk, curDir, (void *) &subBlock);
			cout << "Directory " << command.file_name << " is created." << endl; 
		}
		else if(!there && !isSpace)
			cout << "There is no space for the directory to be created in the current directory. " << endl;
		else
			cout << "Directory " << command.file_name << " is already created." << endl; 
	}
}

//output the current subdirs and subfiles
void ls(dirblock_t curBlock, cmd_t command, int disk){
	cout << "Name  Block   Type   Bytes  NumBlocks(Full Blocks)  LinkCount" << endl;
	inode_t tempBlock1;
	int numBlocks;
	int count = 0;
	for(int i = 0; i < MAX_FILES; i++)
	{
		count++;
		if(isDir(curBlock.dir_entries[i].block_num, disk) && curBlock.dir_entries[i].block_num != 0)
		{
			cout << curBlock.dir_entries[i].name << "     " << curBlock.dir_entries[i].block_num 
				<< "      " << "dir" << endl;
		}
		else if(curBlock.dir_entries[i].block_num != 0)
		{
			read_disk_block(disk, curBlock.dir_entries[i].block_num, (void *) &tempBlock1);
			numBlocks = ((tempBlock1.size/BLOCK_SIZE))+FILE_BLOCK;
			cout << curBlock.dir_entries[i].name << "     " << curBlock.dir_entries[i].block_num 
				<< "      " << "file" <<"      "<< tempBlock1.size << "      " << numBlocks <<
				 "      " << tempBlock1.link_count << endl;
		}
	}
	cout << endl;
}

//turn the current directory into the parameter that is passed
void cd(dirblock_t curBlock, cmd_t command, short&curDir, int disk){
	bool found = false;
	bool dir = false;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, curBlock.dir_entries[i].name) == 0)
		{
			found = true;
			if(isDir(curBlock.dir_entries[i].block_num, disk)){
				curDir = curBlock.dir_entries[i].block_num;
				dir = true;
			}
		}
	}
	if(found && dir)
		cout << "Directory " << command.file_name << " entered." << endl;
	else if(found && !dir)
		cout << "This is not a directory. Cannot enter." << endl;
	else
		cout << "Directory not found. " << endl;
}

//remove a directory
void rmDir(dirblock_t curBlock, cmd_t command, short curDir, int disk){
	char name[MAX_FNAME_SIZE] = "";
	bool found = false;
	bool dir = false;
	bool empty = false;
	dirblock_t tempBlock;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, curBlock.dir_entries[i].name) == 0)
		{	found = true;
			if(isDir(curBlock.dir_entries[i].block_num, disk)){
				dir = true;
				read_disk_block(disk, curBlock.dir_entries[i].block_num, (void*) &tempBlock);
				if(tempBlock.num_entries == 0){
					reclaim_block(disk, curBlock.dir_entries[i].block_num);
					empty = true;
					curBlock.num_entries--;
					curBlock.dir_entries[i].block_num = 0;
					strcpy(curBlock.dir_entries[i].name, name);
					write_disk_block(disk, curDir, (void *) &curBlock);
				}
			}
		}
	}
	if(found && empty)
		cout << "Directory " << command.file_name << " deleted." << endl;
	else if(found && !dir)
		cout << "This file is not a directory. Please use rm." << endl;
	else if(found && !empty)
		cout << "Directory " << command.file_name << " is not empty. Cannot delete." << endl;
	else
		cout << "Directory not found. " << endl;
}

//create a new iNode file
void createF(dirblock_t curBlock, cmd_t command, short curDir, int disk){
	inode_t newFile;
	bool there = false;
	bool isSpace = false;
	int newBlockNum;
	int emptyIndex;
	char empty[BLOCK_SIZE] = "";
	datablock_t tempDa;
	int tempAdd;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, curBlock.dir_entries[i].name) == 0 && !isDir(curBlock.dir_entries[i].block_num, disk))
			there = true;
		if(curBlock.dir_entries[i].block_num == 0){
			emptyIndex = i;
			isSpace = true;
		}
	}

	if(getTaken(disk) >= 32768)
		isSpace = false;

	if(!there && isSpace){
		newFile = create();
		newBlockNum = get_free_block(disk);

		strcpy(curBlock.dir_entries[emptyIndex].name, command.file_name);
		curBlock.dir_entries[emptyIndex].block_num = newBlockNum;
		curBlock.num_entries++;
		tempAdd = newBlockNum;
		for(int j = 0; j < MAX_BLOCKS; j++)
		{
			newFile.blocks[j] = 0;
		}
		newBlockNum = get_free_block(disk);
		newFile.blocks[0] = newBlockNum;
		strcpy(tempDa.data, empty);

		write_disk_block(disk, curDir, (void *) &curBlock);
		write_disk_block(disk, tempAdd, (void *) &newFile);
		write_disk_block(disk, newBlockNum, (void*)&tempDa);

		cout << "File " << command.file_name << " is now created. " << endl;
	}
	else if(!there && !isSpace)
		cout << "There is no space for the file to be created in the current directory. " << endl;
	else
		cout << "File " << command.file_name << " is already created. " << endl;
}

//output to the current iNode that is passed in
void append(dirblock_t curBlock, cmd_t command, int disk){
	datablock_t tempData;
	int sizeStr = (int)strlen(command.data);
	int curIndex = 0;
	int pastIndex = 0;
	int fileInd = 0;
	inode_t tempFile;
	int newBlockNum;
	bool start = true;
	bool space = true;
	bool file = false;
	bool found = false;

	//char *temp;
	//
	//temp = strchr(command.data, '\n');
	//if(temp!= NULL) 
	//	*temp = 0;

	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, curBlock.dir_entries[i].name) == 0)
		{
			found = true;
			if(!isDir(curBlock.dir_entries[i].block_num, disk)){
				file = true;
				read_disk_block(disk, curBlock.dir_entries[i].block_num, (void*)&tempFile);
				
				if((ceil(tempFile.size+sizeStr)) > MAX_FILE_SIZE)
					space = false;
				
				for(int k = 0; k < MAX_BLOCKS; k++){
					if(tempFile.blocks[k] != 0)
					{
						curIndex = k;
						pastIndex = curIndex;
					}
				}
				read_disk_block(disk, tempFile.blocks[curIndex], (void*) &tempData);
					if(space){
						for(int j = 0; j < sizeStr; j++)
						{
							
							if(curIndex != pastIndex)
							{
								read_disk_block(disk, tempFile.blocks[curIndex], (void*) &tempData);
								pastIndex = curIndex;
							}

							fileInd = (tempFile.size%128);

							if(tempFile.size % 128 == 0 && !start && tempFile.size != MAX_FILE_SIZE)
							{
								newBlockNum = get_free_block(disk);
								curIndex++;
								read_disk_block(disk, tempFile.blocks[curIndex], (void*)&tempData);
								tempData.data[0] = command.data[j];
								tempFile.size++;
								write_disk_block(disk, newBlockNum, (void*) &tempData);
								tempFile.blocks[curIndex] = newBlockNum;
							}
							else{
								tempData.data[fileInd] = command.data[j];
								tempFile.size++;
								write_disk_block(disk, tempFile.blocks[curIndex], (void*)&tempData);
							}
							start = false;

						}
						
					}
					write_disk_block(disk, curBlock.dir_entries[i].block_num, (void*)&tempFile);
					
			}
		}

	}
	if(!found)
		cout << "File was not found!" << endl;
	else if(found && !file)
		cout << "This is a directory. Cannot output contents. " << endl;
	else if(file && !space)
		cout << "No more free space available in this file! " << endl;

}

//output the given iNode block
void cat(dirblock_t curBlock, cmd_t command, int disk){
	inode_t tempFile;
	
	bool found = false;
	bool file = false;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, curBlock.dir_entries[i].name) == 0)
		{
			found = true;
			if(!isDir(curBlock.dir_entries[i].block_num, disk)){
				file = true;
				read_disk_block(disk, curBlock.dir_entries[i].block_num, (void*)&tempFile);
				cout << "The file " << command.file_name <<" holds: ";
				for(int j = 0; j < MAX_BLOCKS; j++)
				{
					if(tempFile.blocks[j] > 0)
					{
						datablock_t tempD;
						read_disk_block(disk, tempFile.blocks[j], (void*)&tempD);
						int dataSize = strlen(tempD.data);
						if(dataSize > BLOCK_SIZE)
							for(int k = 0; k < BLOCK_SIZE; k++)
								cout << tempD.data[k];
						else
							for(int k = 0; k < strlen(tempD.data); k++)
								cout << tempD.data[k];
					}
				}
			}
		}
	}
	if(found && !file)
		cout << "This is not a file! Cannot output contents.";
	else if(!found)
		cout << "File does not exist.";

	cout << endl;
}


//Create a link named <dest> to an existing file named <src>
void link(dirblock_t curBlock, cmd_t command, short curDir, int disk){
	inode_t sourceFile;
	inode_t destFile;
	bool there = false;
	bool there2 = false;
	bool homo = false;
	bool isSpace = false;
	int newBlockNum;
	int sourceBlockNum;
	int emptyIndex;
	
	if(strcmp(command.data, command.file_name) == 0)
		homo = true;
	
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, curBlock.dir_entries[i].name) == 0 && !isDir(curBlock.dir_entries[i].block_num, disk)){
			there = true;
			sourceFile = getiNode(curBlock.dir_entries[i].block_num, disk);
			sourceBlockNum = curBlock.dir_entries[i].block_num;
		}
		if(strcmp(command.data, curBlock.dir_entries[i].name) == 0 && !isDir(curBlock.dir_entries[i].block_num, disk))
			there2 = true;
		if(curBlock.dir_entries[i].block_num == 0){
			emptyIndex = i;
			isSpace = true;
		}
	}

	if(getTaken(disk) >= 32768)
		isSpace = false;
	
	if(!there){
		cout << "File " << command.file_name << " does not exist. " << endl;
	}
	else if(!there2 && isSpace){
		if(homo){
			cout << "source and dest name can't be the same. " <<endl; 
		}
		else{
			destFile = create();
			newBlockNum = get_free_block(disk);
			strcpy(curBlock.dir_entries[emptyIndex].name, command.data);
			curBlock.dir_entries[emptyIndex].block_num = newBlockNum;
			curBlock.num_entries++;
		
			destFile.magic = sourceFile.magic;
			destFile.size = sourceFile.size;
			memcpy(destFile.blocks, sourceFile.blocks, MAX_BLOCKS);
			destFile.link_count++;
			sourceFile.link_count++;
		
			write_disk_block(disk, curDir, (void *) &curBlock);
			write_disk_block(disk, newBlockNum, (void *) &destFile);
			write_disk_block(disk, sourceBlockNum, (void *) &sourceFile);
			
			cout << "File " << command.data << " is now created. " << endl;
		}
	}
	else if(!there2 && !isSpace)
		cout << "There is no space for the file to be created in the current directory. " << endl;
	else
		cout << "File " << command.data << " is already created. " << endl;
}


//Remove a link to the name. (When the link count drops to 0, delete the file)
void unlink(dirblock_t curBlock, cmd_t command, short curDir, int disk){
	inode_t sourceFile;
	bool there = false;
	int sourceBlockNum;
	
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, curBlock.dir_entries[i].name) == 0 && !isDir(curBlock.dir_entries[i].block_num, disk)){
			there = true;
			sourceFile = getiNode(curBlock.dir_entries[i].block_num, disk);
			sourceBlockNum = curBlock.dir_entries[i].block_num;
		}
	}
	
	if(!there){
		cout << "File " << command.file_name << " does not exist. " << endl;
	}
	else{
		if(sourceFile.link_count == 1){
			rm(curBlock, command, sourceBlockNum, disk);
		}
		else{
			sourceFile.link_count--;
			write_disk_block(disk, sourceBlockNum, (void *) &sourceFile);
			cout << "File " << command.data << " is unlinked. " << endl;
		}		
	}
}


//remove the passed in block
void rm(dirblock_t curBlock, cmd_t command, short curDir, int disk){
	char name[MAX_FNAME_SIZE] = "";
	char empty[BLOCK_SIZE] = "";
	bool found = false;
	inode_t tempFile;
	inode_t tempFile2;
	datablock_t dataTemp;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, curBlock.dir_entries[i].name) == 0)
		{
			if(!isDir(curBlock.dir_entries[i].block_num, disk)){
				read_disk_block(disk, curBlock.dir_entries[i].block_num, (void*) &tempFile);
				for(int j = 0; j < ((tempFile.size/BLOCK_SIZE)+FILE_BLOCK); j++)
				{
					if(tempFile.blocks[j] != 0){
						read_disk_block(disk, tempFile.blocks[j], (void*)&dataTemp);
						strcpy(dataTemp.data, empty);
						write_disk_block(disk, tempFile.blocks[j], (void*)&dataTemp);
						cout << tempFile.blocks[j] << endl;
						reclaim_block(disk, tempFile.blocks[j]);
						tempFile.blocks[j] = 0; 
					}

				}
				reclaim_block(disk, curBlock.dir_entries[i].block_num);

				found = true;
				curBlock.num_entries--;
				curBlock.dir_entries[i].block_num = 0;
				strcpy(curBlock.dir_entries[i].name, name);
				write_disk_block(disk, curDir, (void *) &curBlock);
			}
		}
	}
	if(found)
		cout << "File " << command.file_name << " deleted." << endl;
	else
		cout << "File not found. " << endl;	
}

//return the space left in the disk
void space(int disk)
{
	int totalSpace;
	int available =0;
	int taken = 0;
	int byte; 
	int bit;
	superblock_t supBlock;

	read_disk_block(disk, 0, (void*)&supBlock);

	for (byte = 0; byte < BLOCK_SIZE; byte++) {
		if (supBlock.bitmap[byte] != 0xFF) {

			// loop to check each bit
			for (bit = 0; bit < BYTE_SIZE; bit++) {
				int mask = 1 << bit;	// used to determine if bit is set or not
				if (mask & ~supBlock.bitmap[byte])
					available++;
				else
					taken++;
			}
		}
		else
			taken += BYTE_SIZE;
	}
	totalSpace = (available+taken);
	cout << "Available blocks: " << available  << endl;
	cout << "Taken blocks: " << taken << endl;
	cout << "Total blocks: " << totalSpace << endl;
}

//return the amount of space taken in the disk
int getTaken(int disk)
{
	int totalSpace;
	int available = 0;
	int taken = 0;
	int byte; 
	int bit;
	superblock_t supBlock;

	read_disk_block(disk, 0, (void*)&supBlock);

	for (byte = 0; byte < BLOCK_SIZE; byte++) {
		if (supBlock.bitmap[byte] != 0xFF) {

			// loop to check each bit
			for (bit = 0; bit < BYTE_SIZE; bit++) {
				int mask = 1 << bit;	// used to determine if bit is set or not
				if (!(mask & ~supBlock.bitmap[byte]))
					taken++;
			}
		}
		else
			taken += BYTE_SIZE;
	}
	return taken;
}

//Make a new file system, i.e., format the disk so that it is
//ready for other file system operations.
int mkfs()
{
	int fd;		// file descriptor for disk
	bool new_disk;	// set if new disk was created
	int i;		// loop traversal variable

	struct superblock_t super_block;	// used to initialize block 0
	struct dirblock_t dir_block;		// used to initialize block 1
	struct datablock_t data_block;	// used to initialize other blocks

	// Mount the disk
	new_disk = mount_disk(DISK_NAME, &fd);

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


