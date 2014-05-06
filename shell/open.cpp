/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//create a new inode file

using namespace std;


void open(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd, int &gfd, int &flag){
	File_Inode newFile;
	bool there = false;
	bool isSpace = false;
	int newBlockNum;
	int emptyIndex;
	char empty[BLOCK_SIZE] = "";
	Data_Block tempDa;
	int tempAdd;
	
	if(gfd != fd) {
		cout << "wrong fd! Please close other file first" <<endl;
	}
	else {
		if(strcmp(strtok(command.data, " \t\n"), "r") == 0)
			flag = 1;
		else if(strcmp(strtok(command.data, " \t\n"), "w") == 0)
			flag = 2;
		else if(strcmp(strtok(command.data, " \t\n"), "rw") == 0)
			flag = 3;
	
		for(int i = 0; i < MAX_FILES; i++)
		{
			if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0 && !IsDir(current_block.dir_entries[i].block_num, fd)){
				there = true;
				tempAdd = current_block.dir_entries[i].block_num;
			}
			if(current_block.dir_entries[i].block_num == 0){
				emptyIndex = i;
				isSpace = true;
			}
		}

		if(GetTaken(fd) >= 32768)
			isSpace = false;

		if(!there && isSpace && ((flag == 2) || (flag == 3))){
			newFile = Create();
			newBlockNum = AddNewBlock(fd);

			strcpy(current_block.dir_entries[emptyIndex].name, command.file_name);
			current_block.dir_entries[emptyIndex].block_num = newBlockNum;
			current_block.num_entries++;
			tempAdd = newBlockNum;
			/*for(int j = 0; j < MAX_BLOCKS; j++)
			{
				newFile.blocks[j] = 0;
			}*/
			newBlockNum = AddNewBlock(fd);
			newFile.blocks[0] = newBlockNum;
			time(&newFile.timer);
			strcpy(tempDa.data, empty);

			WriteDisk(fd, current_dir, (void *) &current_block);
			WriteDisk(fd, tempAdd, (void *) &newFile);
			WriteDisk(fd, newBlockNum, (void*)&tempDa);
		
			gfd = tempAdd; //save the current file descriptor for write operation
			cout << "File " << command.file_name << " is now created. " << endl;
			cout << "fd is " << tempAdd << endl;
		}
		else if(!there && !isSpace)
			cout << "There is no space for the file to be created in the current directory. " << endl;
		
		else if(there){
			gfd = tempAdd;
			//cout << "File " << command.file_name << " is already created. " << endl;
			cout << "fd is " << tempAdd << endl;
		}
			
		else
			cout << "File does not exist, flag should not be r. " << endl;
	}
}

//initialize a iNode
File_Inode Create(){
	File_Inode tempINode;
	tempINode.magic = INODE_MAGIC_NUM;
	tempINode.size = 0;
	tempINode.link_count = 1;
	tempINode.timer = 0;
	for(int i = 0; i < MAX_BLOCKS; i++)
		tempINode.blocks[i] = 0;
	for(int i = 0; i < 3; i++)
		tempINode.links[i] = -1;

	return tempINode;
}

