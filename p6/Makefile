# Compiling
CC = gcc
CFLAGS += -Wall -std=c99 -g
LDLIBS +=

# Build all programs
all: driver sort segTreeTest

# Build the final program
driver: driver.o segTree.o input.o

# Build the sort program
sort: sort.o segTree.o

# Build the segTreeTest program
segTreeTest: segTreeTest.o segTree.o

# Build driver.o
driver.o: driver.c segTree.h input.h

# Build segTree.o
segTree.o: segTree.c segTree.h

# Build input.o
input.o: input.c input.h

# Build sort.o
sort.o: sort.c segTree.h

# Build segTreeTest.o
segTreeTest.o: segTreeTest.c segTree.h

# Clean for object files and executable
clean:
	rm -f driver.o segTree.o input.o sort.o segTreeTest.o driver sort segTreeTest
	rm -f *.gcda *.gcno *.gcov
	rm -f output.txt stderr.txt stdout.txt
