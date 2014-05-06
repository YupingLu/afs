/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//remove the iNode file given

using namespace std;


void rm(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd){
	char name[MAX_FNAME_SIZE] = "";
	char empty[BLOCK_SIZE] = "";
	bool found = false;
	File_Inode tempFile;
	File_Inode tempFile2;
	Data_Block dataTemp;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0)
		{
			if(!IsDir(current_block.dir_entries[i].block_num, fd)){
				ReadDisk(fd, current_block.dir_entries[i].block_num, (void*) &tempFile);
				
				for(int w=0; w < tempFile.link_count-1; w++)
				{
					tempFile2 = GetInode(tempFile.links[w], fd);
					for(int s=0; s<tempFile2.link_count-1; s++)
					{
						if(tempFile2.links[s] == current_block.dir_entries[i].block_num)
						{
							tempFile2.links[s] = tempFile2.links[tempFile2.link_count-1];
							tempFile2.links[tempFile2.link_count-1] = -1;
						}
					}
					if(tempFile2.link_count > 1)
						tempFile2.link_count--;

					WriteDisk(fd, tempFile.links[w], (void *) &tempFile2);
				}
				
				for(int j = 0; j < ((tempFile.size/BLOCK_SIZE)+FILE_BLOCK); j++)
				{
					if(tempFile.blocks[j] != 0){
						ReadDisk(fd, tempFile.blocks[j], (void*)&dataTemp);
						strcpy(dataTemp.data, empty);
						WriteDisk(fd, tempFile.blocks[j], (void*)&dataTemp);
						cout << tempFile.blocks[j] << endl;
						FreeBlock(fd, tempFile.blocks[j]);
						tempFile.blocks[j] = 0; 
					}

				}
				FreeBlock(fd, current_block.dir_entries[i].block_num);

				found = true;
				current_block.num_entries--;
				current_block.dir_entries[i].block_num = 0;
				strcpy(current_block.dir_entries[i].name, name);
				WriteDisk(fd, current_dir, (void *) &current_block);
			}
		}
	}
	if(found)
		cout << "File " << command.file_name << " deleted." << endl;
	else
		cout << "File not found. " << endl;	
}

