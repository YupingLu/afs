/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */



//List the contents of the current directory in a tree-format.
//For each file listed, its date file size should be included.


using namespace std;

void tree(Dir_Inode current_block, Cmd_Set command, int fd, int count){
	File_Inode tempBlock1;
	Dir_Inode temp_dir;
	int numBlocks;
	char star = '-';
	count++;	
	
	for(int i = 0; i < MAX_FILES; i++)
	{
		if(IsDir(current_block.dir_entries[i].block_num, fd) && current_block.dir_entries[i].block_num != 0)
		{
			ReadDisk(fd, current_block.dir_entries[i].block_num, (void *) &temp_dir);
			cout << string(count*3, star);
			cout << current_block.dir_entries[i].name << " dir " << ctime(&temp_dir.timer);
			
			tree(temp_dir, command, fd, count);
		}
		else if(current_block.dir_entries[i].block_num != 0)
		{
			ReadDisk(fd, current_block.dir_entries[i].block_num, (void *) &tempBlock1);
			//numBlocks = ((tempBlock1.size/BLOCK_SIZE))+FILE_BLOCK;
			cout << string(count*3, star);
			cout << current_block.dir_entries[i].name << " file " << tempBlock1.size << " " << ctime(&tempBlock1.timer);
			
		}
	}
	
}
