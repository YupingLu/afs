/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//append to the current file

using namespace std;

void write(Dir_Inode current_block, Cmd_Set command, int fd){
	Data_Block tempData;
	int sizeStr = (int)strlen(command.data);
	int curIndex = 0;
	int pastIndex = 0;
	int fileInd = 0;
	File_Inode tempFile;
	int newBlockNum;
	bool start = true;
	bool space = true;
	bool file = false;
	bool found = false;


	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0)
		{
			found = true;
			if(!IsDir(current_block.dir_entries[i].block_num, fd)){
				file = true;
				ReadDisk(fd, current_block.dir_entries[i].block_num, (void*)&tempFile);
				
				if((ceil(tempFile.size+sizeStr)) > MAX_FILE_SIZE)
					space = false;
				
				for(int k = 0; k < MAX_BLOCKS; k++){
					if(tempFile.blocks[k] != 0)
					{
						curIndex = k;
						pastIndex = curIndex;
					}
				}
				ReadDisk(fd, tempFile.blocks[curIndex], (void*) &tempData);
				if(space){
					for(int j = 0; j < sizeStr; j++)
					{
							
						if(curIndex != pastIndex)
						{
							ReadDisk(fd, tempFile.blocks[curIndex], (void*) &tempData);
							pastIndex = curIndex;
						}

						fileInd = (tempFile.size%128);

						if(tempFile.size % 128 == 0 && !start && tempFile.size != MAX_FILE_SIZE)
						{
							newBlockNum = AddNewBlock(fd);
							curIndex++;
							ReadDisk(fd, tempFile.blocks[curIndex], (void*)&tempData);
							tempData.data[0] = command.data[j];
							tempFile.size++;
							WriteDisk(fd, newBlockNum, (void*) &tempData);
							tempFile.blocks[curIndex] = newBlockNum;
						}
						else{
							tempData.data[fileInd] = command.data[j];
							tempFile.size++;
							WriteDisk(fd, tempFile.blocks[curIndex], (void*)&tempData);
						}
						start = false;

					}
						
				}
				WriteDisk(fd, current_block.dir_entries[i].block_num, (void*)&tempFile);
					
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
