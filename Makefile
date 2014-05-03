all: afs
afs: afs.cpp
	g++ -g -o afs afs.cpp
	rm -f virtual_disk
clean:
	rm -f *.o afs
	rm -f virtual_disk
