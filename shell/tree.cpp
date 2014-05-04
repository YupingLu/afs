/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */



//List the contents of the current directory in a tree-format.
//For each file listed, its date file size should be included.


using namespace std;

void tree(Dir_Inode current_block, Cmd_Set command, int fd){
	//File_Inode tempBlock1;
	Dir_Inode temp_dir;
	int numBlocks;
	int count = 0;
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(IsDir(current_block.dir_entries[i].block_num, fd) && current_block.dir_entries[i].block_num != 0)
		{
			cout << current_block.dir_entries[i].name << endl;
			count++;
			ReadDisk(fd, current_block.dir_entries[i].block_num, (void *) &temp_dir);
			tree(temp_dir, command, fd);
		}
		else if(current_block.dir_entries[i].block_num != 0)
		{
			//ReadDisk(fd, current_block.dir_entries[i].block_num, (void *) &tempBlock1);
			//numBlocks = ((tempBlock1.size/BLOCK_SIZE))+FILE_BLOCK;
			cout << current_block.dir_entries[i].name << endl;
			count++;
		}
	}
	if(count == 0)
		cout << "Empty directory." <<endl;
}
