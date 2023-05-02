CC=g++
FLAGS=-Wall -g -std=c++14
TARGET=create_text radix main

WORST_ENTRY=ABACABADABACABAEABACABADABACABA
DNA_STRING=AGTTTTCATTCTGACTGAACAGCTTTTCATTCTGACTGCAAC
STRING=banaananaanana

TEXT=$(DNA_STRING)
NUMBER_OF_PERMUTATIONS=0
FILE_NAME=in-plain-text.txt#"../dataset/pizza&chilli/english.50MB.txt"
COMPRESSED_FILE=compressed-file
OUT_PLAIN_TEXT_FILE=out-plain-text.txt
IN_PLAIN_TEXT_FILE=in-plain-text.txt

all: $(TARGET)

fraenkel: ../../fraenkel
	./../../fraenkel 14 $(IN_PLAIN_TEXT_FILE)

create_text: ../generate_text.cpp
	$(CC) ../generate_text.cpp -o ../generate_text
	.././generate_text $(TEXT) $(NUMBER_OF_PERMUTATIONS) in-plain-text.txt

radix: radix.cpp radix.hpp
	$(CC) -c radix.cpp -o radix.o $(FLAGS)

main: main.cpp radix
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main radix.o main.o  $(FLAGS)

test: radix radix_test.cpp
	$(CC) -c radix_test.cpp -o radix_test.o $(FLAGS)
	$(CC) -o radix_test radix.o radix_test.o -lgtest -lgtest_main -lpthread $(FLAGS)
	./radix_test $(FILE_NAME)

compress:  radix main.cpp clean
	$(CC) -c radix.cpp -o radix.o $(FLAGS)
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main radix.o main.o  $(FLAGS)
	valgrind --leak-check=full ./main $(FILE_NAME) $(COMPRESSED_FILE) e

decompress: radix main.cpp clean
	$(CC) -c radix.cpp -o radix.o $(FLAGS)
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main radix.o main.o  $(FLAGS)
	./main $(COMPRESSED_FILE) $(OUT_PLAIN_TEXT_FILE) d

clean:
	rm -rf *.o radix_test main ../generate_text grammar-test