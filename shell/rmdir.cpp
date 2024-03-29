/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//remove a directory

using namespace std;


void rmdir(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd){
	char name[MAX_FNAME_SIZE] = "";
	bool found = false;
	bool dir = false;
	bool empty = false;
	Dir_Inode tempBlock;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0)
		{	found = true;
			if(IsDir(current_block.dir_entries[i].block_num, fd)){
				dir = true;
				ReadDisk(fd, current_block.dir_entries[i].block_num, (void*) &tempBlock);
				if(tempBlock.num_entries == 0){
					FreeBlock(fd, current_block.dir_entries[i].block_num);
					empty = true;
					current_block.num_entries--;
					current_block.dir_entries[i].block_num = 0;
					strcpy(current_block.dir_entries[i].name, name);
					WriteDisk(fd, current_dir, (void *) &current_block);
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


