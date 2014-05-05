/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//Export a file from the current directory to the host machine file system.

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstdlib>

using namespace std;


void output(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd){
	int output_fd;    /* Output file descriptors */
    ssize_t ret_out;    /* Number of bytes returned by write() */
    //char buffer[BLOCK_SIZE];      /* Character buffer */

	File_Inode tempFile;
	bool there = false;
	int datasize;
	
	command.data = strtok(command.data, " \t\n");
	
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0 && !IsDir(current_block.dir_entries[i].block_num, fd))
		{
			ReadDisk(fd, current_block.dir_entries[i].block_num, (void*)&tempFile);
			datasize = tempFile.size;
			there = true;
		}	
	}
	
	if(there){
		/* Create output file descriptor */
		output_fd = open(command.data, O_WRONLY | O_CREAT, 0644);
		if(output_fd == -1){
		    cerr << "Open failure" << endl;
			//exit(-1);
		}else {	
			for(int j = 0; j < MAX_BLOCKS; j++)
			{
				if(tempFile.blocks[j] != 0)
				{
					Data_Block tempD;
					ReadDisk(fd, tempFile.blocks[j], (void*)&tempD);
					
					if(datasize > BLOCK_SIZE)
					{
						ret_out = write(output_fd, &tempD, BLOCK_SIZE);					
						datasize -= BLOCK_SIZE;
					}
					else
					{
						ret_out = write(output_fd, &tempD, datasize);					
					}
				}
			}
		
			cout << "File " << command.data << " is now exported. " << endl;
			/* Close file descriptors */
			close(output_fd);
		}

	}
	else
		cout << "File " << command.file_name << " does not exist or is a directory." << endl;
}

