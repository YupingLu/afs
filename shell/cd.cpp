/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//turn the current directory into the parameter that is passed

using namespace std;


void cd(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd){
	bool found = false;
	bool dir = false;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0)
		{
			found = true;
			if(IsDir(current_block.dir_entries[i].block_num, fd)){
				current_dir = current_block.dir_entries[i].block_num;
				dir = true;
			}
		}
	}
	if(found && dir)
		cout << "Directory " << command.file_name << " entered." << endl;
	else if(found && !dir)
		cout << "This is not a directory. Cannot enter." << endl;
	else
		cout << "Directory not found. " << endl;
}
