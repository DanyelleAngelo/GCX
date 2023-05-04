CC=g++
FLAGS=-Wall -g -std=c++14

WORST_ENTRY=ABACABADABACABAEABACABADABACABA
DNA_STRING=AGTTTTCATTCTGACTGAACAGCTTTTCATTCTGACTGCAAC
STRING=banaananaanana

TEXT=$(STRING)
NUMBER_OF_PERMUTATIONS=0
IN_PLAIN_TEXT_FILE=text/in-plain-text.txt#"../dataset/pizza&chilli/english.50MB.txt"
COMPRESSED_FILE=text/compressed-file
OUT_PLAIN_TEXT_FILE=text/out-plain-text.txt

all: fraenkel compress

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
	./radix_test $(IN_PLAIN_TEXT_FILE)

compress:  radix main clean
	./main $(IN_PLAIN_TEXT_FILE) $(COMPRESSED_FILE) e

decompress: radix main.cpp clean
	./main $(COMPRESSED_FILE) $(OUT_PLAIN_TEXT_FILE) d

clean:
	rm -rf *.o