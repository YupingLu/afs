/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//Copy the file from a source location to the destination
//location. If the <dest> already exists, the program should
//report an error.


using namespace std;


void cp(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd){
	File_Inode sourceFile;
	File_Inode destFile;
	bool there = false;
	bool there2 = false;
	bool homo = false;
	bool isSpace = false;
	int newBlockNum;
	int sourceBlockNum;
	int emptyIndex;
	Data_Block tempData;
	int fileInd = 0;
	int datasize = 0;
	
	command.data = strtok(command.data, " \t\n");
	
	if(strcmp(command.data, command.file_name) == 0)
		homo = true;
	
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0 && !IsDir(current_block.dir_entries[i].block_num, fd)){
			there = true;
			sourceFile = GetInode(current_block.dir_entries[i].block_num, fd);
			sourceBlockNum = current_block.dir_entries[i].block_num;
			datasize = sourceFile.size;
		}
		if(strcmp(command.data, current_block.dir_entries[i].name) == 0 && !IsDir(current_block.dir_entries[i].block_num, fd))
			there2 = true;
		if(current_block.dir_entries[i].block_num == 0){
			emptyIndex = i;
			isSpace = true;
		}
	}
	
	if(GetTaken(fd) + datasize >= 32768)
		isSpace = false;
	
	if(!there){
		cout << "File " << command.file_name << " does not exist. " << endl;
	}
	else if(!there2 && isSpace){
		if(homo){
			cout << "source and dest name can't be the same. " <<endl; 
		}
		else{
			destFile = Create();
			newBlockNum = AddNewBlock(fd);
			strcpy(current_block.dir_entries[emptyIndex].name, command.data);
			current_block.dir_entries[emptyIndex].block_num = newBlockNum;
			current_block.num_entries++;
		
			destFile.magic = sourceFile.magic;
			destFile.size = sourceFile.size;
			//memcpy(destFile.blocks, sourceFile.blocks, MAX_BLOCKS);
			destFile.link_count = sourceFile.link_count;
			time(&destFile.timer);
		
			WriteDisk(fd, current_dir, (void *) &current_block);
			WriteDisk(fd, newBlockNum, (void *) &destFile);
			
			for(int j = 0; j < MAX_BLOCKS; j++)
			{
				if(sourceFile.blocks[j] != 0)
				{
					Data_Block tempD;
					Data_Block tempC;
					int tempBlockNum = AddNewBlock(fd);
					destFile.blocks[j] = tempBlockNum;
					ReadDisk(fd, sourceFile.blocks[j], (void*)&tempD);
					
					if(datasize > BLOCK_SIZE)
					{
						for(int k = 0; k < BLOCK_SIZE; k++)
							tempC.data[k] = tempD.data[k];
						
						datasize -= BLOCK_SIZE;
					}
					else
						for(int k = 0; k < datasize; k++)
							tempC.data[k] = tempD.data[k];
					WriteDisk(fd, tempBlockNum, (void*) &tempC);
					
				}
			}
			WriteDisk(fd, newBlockNum, (void*) &destFile);
			
			cout << "File " << command.data << " is now copied. " << endl;
			
		}
	}
	else if(!there2 && !isSpace)
		cout << "There is no space for the file to be created in the current directory. " << endl;
	else
		cout << "File " << command.data << " is already created. " << endl;
}

