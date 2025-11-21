# Compiling
CC = gcc
CFLAGS = -Wall -std=c99 -g

# Build the final program
tcrypt: tcrypt.o TDES.o magic.o IO.o

# Build the TDEStest program
TDEStest: TDEStest.o TDES.o magic.o

# Build tcrypt.o
tcrypt.o: tcrypt.c TDES.h IO.h

# Build TDES.o
TDES.o: TDES.c TDES.h TDESinternal.h magic.h IO.h

# Build TDEStest.o
TDEStest.o: TDEStest.c TDES.h TDESinternal.h

# Build magic.o
magic.o: magic.c magic.h

# Build IO.o
IO.o: IO.c IO.h

# Clean for object files and executable
clean:
	rm -f tcrypt.o TDEStest.o TDES.o magic.o IO.o tcrypt
	rm -f output.txt stdout.txt stderr.txt