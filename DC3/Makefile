CC=g++
FLAGS=-W -Wall
TARGET=dc3 main clean

all: $(TARGET)

dc3: dc3.cpp dc3.hpp
	$(CC) -c dc3.cpp -o dc3.o $(FLAGS)

test: dc3 test.cpp
	$(CC) -c test.cpp -o test $(FLAGS)

main: main.cpp
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main dc3.o main.o  $(FLAGS)
	./main text.txt

clean:
	rm -rf *.o main