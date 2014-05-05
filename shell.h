/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 

Implement shell command
*/

#ifndef SHELL_H
#define SHELL_H

//mkdir
bool IsDir(short block_id, int fd);
Dir_Inode GetSubDir(short block_id, int fd);
Dir_Inode NewDir();
void mkdir(Dir_Inode current_block, Cmd_Set command, int current_dir, int fd);
int GetTaken(int fd);

//space
void space(int fd);

//mkfs
int mkfs();

//rm
void rm(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd);

//unlink
void unlink(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd);
File_Inode GetInode(short block_id, int fd);

//open
void open(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd, int &gfd, int &flag);
File_Inode Create();

//link
void link(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd);

//cat
void cat(Dir_Inode current_block, Cmd_Set command, int fd);

//cd
void cd(Dir_Inode current_block, Cmd_Set command, short &current_dir, int fd);

//rmdir
void rmdir(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd);

//write
void write(Dir_Inode current_block, Cmd_Set command, int fd, int &gfd, int flag);

//ls
void ls(Dir_Inode current_block, Cmd_Set command, int fd);

//close
void close(Cmd_Set command, int fd, int &gfd, int &flag, int &offset);

//read
void read(Cmd_Set command, int fd, int &gfd, int flag, int &offset);

//seek
void seek(Cmd_Set command, int fd, int &gfd, int flag, int &offset);

//stat
void stat(Dir_Inode current_block, Cmd_Set command, int fd);

//cp
void cp(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd);

//tree
void tree(Dir_Inode current_block, Cmd_Set command, int fd, int count);

//import
void import(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd);

//export
void output(Dir_Inode current_block, Cmd_Set command, short current_dir, int fd);

#endif
