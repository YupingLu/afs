/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//Import a file from the host machine file system to the current directory.

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstdlib>

using namespace std;


void import(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd){
	int input_fd;    /* Input file descriptors */
    ssize_t ret_in;    /* Number of bytes returned by read() */
    char buffer[BLOCK_SIZE];      /* Character buffer */

	File_Inode destFile;
	bool there = false;
	bool isSpace = false;
	int newBlockNum;
	int emptyIndex;
	
	command.data = strtok(command.data, " \t\n");
	
	
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.data, current_block.dir_entries[i].name) == 0 && !IsDir(current_block.dir_entries[i].block_num, fd))
			there = true;
		if(current_block.dir_entries[i].block_num == 0){
			emptyIndex = i;
			isSpace = true;
		}
	}
	
	if(GetTaken(fd) >= 32768)
		isSpace = false;
	
	if(!there && isSpace){
		/* Create input file descriptor */
		input_fd = open (command.file_name, O_RDONLY);
		if (input_fd == -1) {
			cerr << "Open failure" << endl;
			//exit(-1);
		}else {
			int count = 0;
			destFile = Create();
			newBlockNum = AddNewBlock(fd);
			strcpy(current_block.dir_entries[emptyIndex].name, command.data);
			current_block.dir_entries[emptyIndex].block_num = newBlockNum;
			current_block.num_entries++;
	
			time(&destFile.timer);
	
			WriteDisk(fd, current_dir, (void *) &current_block);
			WriteDisk(fd, newBlockNum, (void *) &destFile);
			
			/* Copy process */
			while((ret_in = read (input_fd, &buffer, BLOCK_SIZE)) > 0){
			    Data_Block tempD;
				int tempBlockNum = AddNewBlock(fd);
				destFile.blocks[count++] = tempBlockNum;
			    destFile.size += ret_in;
				
				for(int k = 0; k < ret_in; k++)
					tempD.data[k] = buffer[k];
						
				WriteDisk(fd, tempBlockNum, (void*) &tempD);
			}
			
			WriteDisk(fd, newBlockNum, (void*) &destFile);
			cout << "File " << command.file_name << " is now copied. " << endl;
			/* Close file descriptors */
    		close(input_fd);
		}

	}
	else if(!there && !isSpace)
		cout << "There is no space for the file to be created in the current directory. " << endl;
	else
		cout << "File " << command.data << " is already created. " << endl;
}

