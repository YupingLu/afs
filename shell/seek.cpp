/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//read the content of the file with offset

using namespace std;

void seek(Cmd_Set command, int fd, int &gfd, int flag, int &offset){
	File_Inode tempFile;
	int datasize;
	
	int range = atoi(strtok(command.data, " \t\n"));
	
	if( gfd != atoi(command.file_name)) {
		cout << "wrong fd! Please make sure you open file first!" <<endl;
	}
	else {
		if(flag == 1 || flag == 3)
		{
			ReadDisk(fd, gfd, (void*)&tempFile);
			datasize = tempFile.size;
			
			if((offset+range) > datasize) {
				cout<< "Offset is larger than file length." << endl;
			}
			else {
				cout << "The file " << command.file_name <<" holds: \n";
				
				int end = offset + range;
				int div1 = offset / BLOCK_SIZE;
				int div2 = end / BLOCK_SIZE;
				int times = div1 - div2;
				int rem1 = offset % BLOCK_SIZE;
				int rem2 = end % BLOCK_SIZE;
					
				Data_Block tempD;
			
				if(times == 0) 
				{
					ReadDisk(fd, tempFile.blocks[div1], (void*)&tempD);	
					
					for(int i = rem1; i < range+rem1; i++)
					{
						cout << tempD.data[i];
					}
					
				} 
				else 
				{
					ReadDisk(fd, tempFile.blocks[div1], (void*)&tempD);
					for(int i = rem1; i < BLOCK_SIZE; i++)
					{
						cout << tempD.data[i];
					}
					
					for(int j = 1; j < times; j++)
					{
						ReadDisk(fd, tempFile.blocks[div1+j], (void*)&tempD);
						for(int w = 0; w < BLOCK_SIZE; w++)
						{
							cout << tempD.data[w];
						}
					}
					
					ReadDisk(fd, tempFile.blocks[div2], (void*)&tempD);
					for(int i = 0; i < rem2; i++)
					{
						cout << tempD.data[i];
					}
				
				}
				cout << endl;
				offset += range;
			}
		}
		else {
			cout<< "Flag should be r." << endl;
		}
	
	}

}
