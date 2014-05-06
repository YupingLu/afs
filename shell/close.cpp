/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//close the current file

using namespace std;

void close(Cmd_Set command, int fd, int &gfd, int &flag, int &offset){

	if(gfd != atoi(command.file_name))
		cout << "wrong fd!" <<endl;
	else{
		gfd = fd;
		flag = 0;
		offset = 0;
		cout << "file closed!" <<endl;
	}

}
