/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */



//Show the status of the file or directory with name <name>.
//It should display its inode information; whether it is a file
//or directory; its link count; size of the file/directory;
//number of blocks allocated; other information stored about
//this file/directory.


using namespace std;

void stat(Dir_Inode current_block, Cmd_Set command, int fd){
	
	File_Inode tempBlock1;
	int numBlocks;
	bool found = false;
	
	
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0)
		{
			cout << "Name  Block_Id   Type   Bytes  NumBlocks(Full Blocks)  LinkCount" << endl;
			if(IsDir(current_block.dir_entries[i].block_num, fd) && current_block.dir_entries[i].block_num != 0)
			{
				cout << current_block.dir_entries[i].name << "     " << current_block.dir_entries[i].block_num 
					<< "      " << "dir" << endl;
			}
			else if(current_block.dir_entries[i].block_num != 0)
			{
				ReadDisk(fd, current_block.dir_entries[i].block_num, (void *) &tempBlock1);
				numBlocks = ((tempBlock1.size/BLOCK_SIZE))+FILE_BLOCK;
				cout << current_block.dir_entries[i].name << "     " << current_block.dir_entries[i].block_num 
					<< "      " << "file" <<"      "<< tempBlock1.size << "      " << numBlocks <<
					 "      " << tempBlock1.link_count << endl;
			}
			else
				cout << "block is empty" <<endl;
			found = true;
		}
	}
	if(!found)
		cout<< command.file_name << " not found.";
	cout << endl;
}
