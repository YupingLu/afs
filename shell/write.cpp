/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//append to the current file

using namespace std;

void write(Dir_Inode current_block, Cmd_Set command, int fd, int &gfd, int flag){
	Data_Block tempData;
	int sizeStr = (int)strlen(command.data);
	int curIndex = 0;
	int pastIndex = 0;
	int fileInd = 0;
	File_Inode tempFile;
	int newBlockNum;
	bool start = true;
	bool space = true;

	
	if( gfd != atoi(command.file_name)) {
		cout << "wrong fd! Please make sure you open file first!" <<endl;
	}
	else {
		if((flag == 2) || (flag == 3))
		{
	
			ReadDisk(fd, gfd, (void*)&tempFile);
				
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

					fileInd = (tempFile.size % BLOCK_SIZE);

					if(fileInd == 0 && !start && tempFile.size != MAX_FILE_SIZE)
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
			WriteDisk(fd, gfd, (void*)&tempFile);

			if(!space)
				cout << "No more free space available in this file! " << endl;
		}else {
			cout<< "Flag is r, should be w or rw" << endl;
		}
	}

}
