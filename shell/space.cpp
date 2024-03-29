/* Programming Assignment 3: Unix-like File System
Name: Yuping(Allan) Lu
Class: CS560 */


//return the space left in the disk

using namespace std;

void space(int fd)
{
	int totalSpace;
	int available =0;
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
				if (mask & ~supBlock.bitmap[byte])
					available++;
				else
					taken++;
			}
		}
		else
			taken += BYTE_SIZE;
	}
	totalSpace = (available+taken);
	cout << "Available blocks: " << available  << endl;
	cout << "Taken blocks: " << taken << endl;
	cout << "Total blocks: " << totalSpace << endl;
}
