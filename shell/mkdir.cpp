/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */

using namespace std;

//Tell whether a block is a directory
bool IsDir(short block_id, int fd)
{
	Dir_Inode temp_block;
	ReadDisk(fd, block_id, (void*) &temp_block);

	if(temp_block.magic == DIR_MAGIC_NUM)
		return true;
	else
		return false;
}

//Get sub directory
Dir_Inode GetSubDir(short block_id, int fd)
{
	Dir_Inode temp_block;
	ReadDisk(fd, block_id, (void*) &temp_block);

	return temp_block;
}

//intialize a directory and return it
Dir_Inode NewDir(){
	Dir_Inode temp_block;
	temp_block.magic = DIR_MAGIC_NUM;
	temp_block.num_entries = 0;

	return temp_block;
}
				
//create a directory
void mkdir(Dir_Inode current_block, Cmd_Set command, int current_dir, int fd){
	const char *DELIM  = "/"; // delimiters
	int numtokens = 0;		// number of tokens
	char *temp_string;		// temporary string used by strtok
	char *dir1;		// sub directory of current directory
	char *dir2;		// directory in sub directory
	
	// Create new string
	temp_string = (char *) new char[strlen(command.file_name) + 1];
	if (temp_string == NULL) {
		cerr << "new failed" << endl; 
		exit(-1);
	}
	strcpy(temp_string, command.file_name);
	
	// Count the number of tokens in the command */
	if (strtok(temp_string, DELIM) != NULL)
		for (numtokens = 1; strtok(NULL, DELIM) != NULL; numtokens++) ; 
	
	// Extract the data
	strcpy(temp_string, command.file_name);
	dir1 = strtok(temp_string, DELIM);
	if (numtokens > 1) dir2 = strtok(NULL, DELIM);
	if (numtokens == 1) {
		bool there = false;
		bool is_space = false;
		short new_block_id;
		Dir_Inode new_block;


		for(int i = 0; i < MAX_FILES; i++)
		{
			if(strcmp(command.file_name, current_block.dir_entries[i].name) == 0 && IsDir(current_block.dir_entries[i].block_num, fd))
				there = true;
			if(current_block.dir_entries[i].block_num == 0)
				is_space = true;
		}

		if(GetTaken(fd) >= 32768)  //1024
			is_space = false;

		if(!there && is_space){
			new_block = NewDir();
			new_block_id = AddNewBlock(fd);
			new_block.parent_id = current_dir;
			time(&new_block.timer);
			WriteDisk(fd, new_block_id, (void *) &new_block);

			strcpy(current_block.dir_entries[current_block.num_entries].name, command.file_name);
			current_block.dir_entries[current_block.num_entries].block_num = new_block_id;
			current_block.num_entries++;
			WriteDisk(fd, current_dir, (void *) &current_block);
			cout << "Directory " << command.file_name << " is created." << endl; 
		}
		else if(!there && !is_space)
			cout << "There is no space for the directory to be created in the current directory. " << endl;
		else
			cout << "Directory " << command.file_name << " is already created." << endl; 
	}
	else if (numtokens > 1) {
		bool there = false;
		bool is_space = false;
		bool dir_exist = false;
		short new_block_id;
		Dir_Inode sub_block;
		Dir_Inode new_block;
		int tempdir;
		

		for(int i = 0; i < MAX_FILES; i++)
		{
			if(strcmp(dir1, current_block.dir_entries[i].name) == 0 && IsDir(current_block.dir_entries[i].block_num, fd)) 
			{
				dir_exist = true;
				sub_block = GetSubDir(current_block.dir_entries[i].block_num, fd);
				tempdir = current_block.dir_entries[i].block_num;
				
				for(int i = 0; i < MAX_FILES; i++)
				{
					if(strcmp(dir2, sub_block.dir_entries[i].name) == 0 && IsDir(sub_block.dir_entries[i].block_num, fd))
						there = true;
					if(sub_block.dir_entries[i].block_num == 0)
						is_space = true;
				}
			}
		
			
		}

		if(GetTaken(fd) >= 32768)  //1024
			is_space = false;

		if(!dir_exist){
			cout << "Directory " << dir1 << " does not exist." << endl; 
		}
		else if(!there && is_space){
			current_dir = tempdir;
		
			new_block = NewDir();
			new_block_id = AddNewBlock(fd);
			new_block.parent_id = current_dir;
			time(&new_block.timer);
			WriteDisk(fd, new_block_id, (void *) &new_block);
			
			strcpy(sub_block.dir_entries[sub_block.num_entries].name, dir2);
			sub_block.dir_entries[sub_block.num_entries].block_num = new_block_id;
			sub_block.num_entries++;
			WriteDisk(fd, current_dir, (void *) &sub_block);
			cout << "Directory " << command.file_name << " is created." << endl; 
		}
		else if(!there && !is_space)
			cout << "There is no space for the directory to be created in the current directory. " << endl;
		else
			cout << "Directory " << command.file_name << " is already created." << endl; 
	}
}

//return the amount of space taken in the disk
int GetTaken(int fd)
{
	int totalSpace;
	int available = 0;
	int taken = 0;
	int byte; 
	int bit;
	Bitmaps supBlock;

	ReadDisk(fd, 0, (void*)&supBlock);

	for (byte = 0; byte < BLOCK_SIZE; byte++) {
		if (supBlock.bitmap[byte] != 0xFF) {

			// loop to check each bit
			for (bit = 0; bit < BYTE_SIZE; bit++) {
				int mask = 1 << bit;	// used to determine if bit is set or not
				if (!(mask & ~supBlock.bitmap[byte]))
					taken++;
			}
		}
		else
			taken += BYTE_SIZE;
	}
	return taken;
}
