CC = g++
CFLAGS = -g 

test: parallelsort.o sortPrep.o
	$(CC) $(CFLAGS) -o test.exe parallelsort.o sortPrep.o -lm -std=c++0x

parallelsort.o: parallelsort.cpp 
	$(CC) $(CFLAGS) -c parallelsort.cpp -std=c++0x

sortPrep.o: sortPrep.cpp
	$(CC) $(CFLAGS) -c sortPrep.cpp

clean:
	rm -rf *.o
