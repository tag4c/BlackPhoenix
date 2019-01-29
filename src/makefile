 
CC = g++
CFLAGS = -g

convertToBinary: asciiToBinary.o
	$(CC) $(CFLAGS) -o convertToBinary asciiToBinary.o -lm -std=c++0x

asciiToBinary.o: asciiToBinary.cpp
	$(CC) $(CFLAGS) -c asciiToBinary.cpp -std=c++0x

convertToAscii: binaryToAscii.o
	$(CC) $(CFLAGS) -o convertToAscii binaryToAscii.o -lm -std=c++0x

binaryToAscii.o: binaryToAscii.cpp
	$(CC) $(CFLAGS) -c binaryToAscii.cpp -std=c++0x

clean:
	rm -rf *.o
	rm -rf convertToBinary
	rm -rf convertToAscii
	rm -rf ../datafiles/binary/output/*
	rm -rf ../datafiles/ascii/output/*