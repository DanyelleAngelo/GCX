CC=g++
FLAGS=-W -Wall -g -std=c++14
TARGET=dc3 main clean

all: $(TARGET)

create_text: generate_text.cpp
	$(CC) generate_text.cpp -o generate_text
	./generate_text mississippi 45 text-large.txt

dc3: dc3.cpp dc3.hpp
	$(CC) -c dc3.cpp -o dc3.o $(FLAGS)

test: dc3 dc3_test.cpp
	$(CC) -c dc3_test.cpp -o dc3_test.o $(FLAGS)
	$(CC) -o dc3_test dc3.o dc3_test.o -lgtest -lgtest_main -lpthread $(FLAGS)
	./dc3_test

main: dc3 main.cpp
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main dc3.o main.o  $(FLAGS)
	valgrind --leak-check=full ./main text-large.txt

clean:
	rm -rf *.o main dc3_test generate_text
