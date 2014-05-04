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
			
			if(range > datasize) {
				cout<< "Offset is larger than file length." << endl;
			}
			else {
				offset = range;
				cout << "The new offset is " << offset <<endl;
			}
		}
		else {
			cout<< "Flag should be r." << endl;
		}
	
	}

}
