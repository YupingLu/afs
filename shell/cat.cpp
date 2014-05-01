/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//output the given iNode block

using namespace std;


void cat(Dir_Inode current_block, Cmd_Set command, int fd){
	File_Inode tempFile;
	
	bool found = false;
	bool file = false;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0)
		{
			found = true;
			if(!IsDir(current_block.dir_entries[i].block_num, fd)){
				file = true;
				ReadDisk(fd, current_block.dir_entries[i].block_num, (void*)&tempFile);
				cout << "The file " << command.file_name <<" holds: \n";
				for(int j = 0; j < MAX_BLOCKS; j++)
				{
					if(tempFile.blocks[j] > 0)
					{
						Data_Block tempD;
						ReadDisk(fd, tempFile.blocks[j], (void*)&tempD);
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


