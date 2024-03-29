/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 

We use a file to simulate a disk.
*/ 

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstdlib>

using namespace std;

const int BLOCK_SIZE = 4096;	    	 
const int NUM_BLOCKS = (BLOCK_SIZE * 8); 

// Write data to disk  
void WriteDisk(int fd, int block_id, void *block)
{
  off_t offset;
  off_t new_offset;
  ssize_t size; 

  if (block_id < 0 || block_id >= NUM_BLOCKS) {
    cerr << "Invalid block size" << endl;
    exit(-1);
  }

  offset = block_id * BLOCK_SIZE;
  new_offset = lseek(fd, offset, SEEK_SET);
  if (offset != new_offset) {
    cerr << "Seek failure" << endl;
    exit(-1);
  }

  size = write(fd, block, BLOCK_SIZE);
  if (size != BLOCK_SIZE) {
    cerr << "Failed to write entire block" << endl;
    exit(-1);
  }
}

// Read data from data
void ReadDisk(int fd, int block_id, void *block)
{
  off_t offset;
  off_t new_offset;
  ssize_t size; 

  if (block_id < 0 || block_id >= NUM_BLOCKS) {
    cerr << "Invalid block size" << endl;
    exit(-1);
  }

  offset = block_id * BLOCK_SIZE;
  new_offset = lseek(fd, offset, SEEK_SET);
  if (offset != new_offset) {
    cerr << "Seek failure" << endl;
    exit(-1);
  }

  size = read(fd, block, BLOCK_SIZE);
  if (size != BLOCK_SIZE) {
    cerr << "Failed to read entire block" << endl;
    exit(-1);
  }
}

// Mount the virtual disk to start the system
bool MountDrive(const char *file_name, int *fd)
{
  *fd = open(file_name, O_RDWR);
  if (*fd != -1) return false;

  *fd = open(file_name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (*fd == -1) {
    cerr << "Could not create disk" << endl;
    exit(-1);
  }

  return true;
}

// Unmount the virtual disk
void UnmountDrive(int fd)
{
  close(fd);
}


