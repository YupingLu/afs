/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */



//output the current subdirs and subfiles

using namespace std;

void ls(Dir_Inode current_block, Cmd_Set command, int fd){
	cout << "Name  Block_Id   Type   Bytes  NumBlocks(Full Blocks)  LinkCount   Date" << endl;
	File_Inode tempBlock1;
	Dir_Inode tempdir;
	int numBlocks;
	int count = 0;
	for(int i = 0; i < MAX_FILES; i++)
	{
		count++;
		if(IsDir(current_block.dir_entries[i].block_num, fd) && current_block.dir_entries[i].block_num != 0)
		{
			ReadDisk(fd, current_block.dir_entries[i].block_num, (void *) &tempdir);
			cout << current_block.dir_entries[i].name << "     " << current_block.dir_entries[i].block_num 
				<< "      " << "dir" << "      " << "        "  << "       " << "       "<< ctime(&tempdir.timer);
		}
		else if(current_block.dir_entries[i].block_num != 0)
		{
			ReadDisk(fd, current_block.dir_entries[i].block_num, (void *) &tempBlock1);
			numBlocks = ((tempBlock1.size/BLOCK_SIZE))+FILE_BLOCK;
			cout << current_block.dir_entries[i].name << "     " << current_block.dir_entries[i].block_num 
				<< "      " << "file"  <<"      "<< tempBlock1.size << "      " << numBlocks << "      " << tempBlock1.link_count <<"      " << ctime(&tempBlock1.timer);
		}
	}
	cout << endl;
}
