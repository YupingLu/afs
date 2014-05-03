/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//Remove a link to the name. (When the link count drops to 0, delete the file)

using namespace std;

void unlink(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd){
	File_Inode sourceFile;
	bool there = false;
	int sourceBlockNum;

	char name[MAX_FNAME_SIZE] = "";
	char empty[BLOCK_SIZE] = "";
	
	Data_Block dataTemp;
	int tmp;
	
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0 && !IsDir(current_block.dir_entries[i].block_num, fd)){
			there = true;
			sourceFile = GetInode(current_block.dir_entries[i].block_num, fd);
			sourceBlockNum = current_block.dir_entries[i].block_num;
			tmp = i;
		}
	}
	
	if(!there){
		cout << "File " << command.file_name << " does not exist. " << endl;
	}
	else{
		if(sourceFile.link_count == 1){
			//rm(current_block, command, sourceBlockNum, fd);
			//ReadDisk(fd, sourceBlockNum, (void*) &tempFile);
			for(int j = 0; j < ((sourceFile.size/BLOCK_SIZE)+FILE_BLOCK); j++)
			{
				if(sourceFile.blocks[j] != 0){
					ReadDisk(fd, sourceFile.blocks[j], (void*)&dataTemp);
					strcpy(dataTemp.data, empty);
					WriteDisk(fd, sourceFile.blocks[j], (void*)&dataTemp);
					//cout << tempFile.blocks[j] << endl;
					FreeBlock(fd, sourceFile.blocks[j]);
					sourceFile.blocks[j] = 0; 
				}
			}
			FreeBlock(fd, sourceBlockNum);

			//found = true;
			current_block.num_entries--;
			current_block.dir_entries[tmp].block_num = 0;
			strcpy(current_block.dir_entries[tmp].name, name);
			WriteDisk(fd, current_dir, (void *) &current_block);
			cout << "File " << command.file_name << " deleted." << endl;
		}
		else if(sourceFile.link_count > 1){
			sourceFile.link_count--;
			WriteDisk(fd, sourceBlockNum, (void *) &sourceFile);
			cout << "File " << command.file_name << " is unlinked. " << endl;
		}		
	}
}

//Get a iNode
File_Inode GetInode(short block_id, int fd){
	File_Inode temp_inode;
	ReadDisk(fd, block_id, (void*) &temp_inode);

	return temp_inode;
}
