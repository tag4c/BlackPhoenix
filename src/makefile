
CC = mpic++
CFLAGS = -g 

parallelsort: parallelsort.o readData.o sortPrep.o writeData.o findPercentile.o globalPosition.o sperateArray.o
	$(CC) $(CFLAGS) -o parallelsort parallelsort.o readData.o sortPrep.o writeData.o findPercentile.o globalPosition.o sperateArray.o -lm -std=c++0x

parallelsort.o: parallelsort.cpp
	$(CC) $(CFLAGS) -c parallelsort.cpp -std=c++0x

readData.o: readData.cpp
	$(CC) $(CFLAGS) -c readData.cpp -std=c++0x

writeData.o: writeData.cpp
	$(CC) $(CFLAGS) -c writeData.cpp -std=c++0x

findPercentile.o: findPercentile.cpp
	$(CC) $(CFLAGS) -c findPercentile.cpp -std=c++0x

sortPrep.o: sortPrep.cpp
	$(CC) $(CFLAGS) -c sortPrep.cpp -std=c++0x

globalPosition.o: globalPosition.cpp
	$(CC) $(CFLAGS) -c globalPosition.cpp -std=c++0x

sperateArray.o: sperateArray.cpp
	$(CC) $(CFLAGS) -c sperateArray.cpp -std=c++0x

test: test.o readData.o
	$(CC) $(CFLAGS) -o test test.o readData.o -lm -std=c++0x

test.o: test.cpp
	$(CC) $(CFLAGS) -c test.cpp -std=c++0x

clean:
	rm -rf *.o

