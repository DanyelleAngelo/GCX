CC=g++
FLAGS=-W -Wall -std=c++14 -g -fsanitize=address
TARGET= create_text dc3 test clean
LIBS=-I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64
MACROS:= DSA12=0 DRADIX=0 DREDUCED=0 DSA0=0 DMERGE=0 DMAPPING=0

all: $(TARGET)

fraenkel: ../../fraenkel
	./../../fraenkel 25 large_text.txt

dc3: dc3.cpp dc3.hpp
	$(CC) -c dc3.cpp -o dc3.o $(addprefix -D, $(MACROS)) $(FLAGS)

test: dc3 dc3_test.cpp
	$(CC) -c dc3_test.cpp -o dc3_test.o $(FLAGS)
	$(CC) -o dc3_test dc3.o dc3_test.o -lgtest -lgtest_main -lpthread $(FLAGS)
	./dc3_test

main:  dc3 main.cpp
	$(CC) -c main.cpp -D DEBUG=1 $(FLAGS)
	$(CC) -o main dc3.o main.o $(FLAGS)
	./main $(FILE)

clean:
	rm -rf *.o main dc3_test generate_text
