CC=g++
FLAGS=-Wall -g -std=c++14
LIBS=-I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

WORST_ENTRY=ABACABADABACABAEABACABADABACABA
DNA_STRING=AGTTTTCATTCTGACTGAACAGCTTTTCATTCTGACTGCAAC
STRING=banaananaanana

TEXT=$(DNA_STRING)
NUMBER_OF_PERMUTATIONS=0

DIR=../dataset/pizza_chilli
FILE=dna.001.1
IN_PLAIN_TEXT_FILE=$(DIR)/$(FILE)
COMPRESSED_FILE=text/$(FILE)-compressed-elias
OUT_PLAIN_TEXT_FILE=text/$(FILE)-plain-text-elias

all: fraenkel compress

fraenkel: ../../fraenkel
	./../../fraenkel 20 $(IN_PLAIN_TEXT_FILE)

create_text: ../generate_text.cpp
	$(CC) ../generate_text.cpp -o ../generate_text
	.././generate_text $(TEXT) $(NUMBER_OF_PERMUTATIONS) text/in-plain-text.txt

compressor: compressor-int.cpp compressor-int.hpp
	$(CC) -c compressor-int.cpp -o compressor-int.o $(FLAGS) $(LIBS)

main: main.cpp compressor
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main compressor-int.o main.o  $(FLAGS) $(LIBS)

# test: compressor-int radix_test.cpp
# 	$(CC) -c radix_test.cpp -o radix_test.o $(FLAGS)
# 	$(CC) -o radix_test compressor-int.o radix_test.o -lgtest -lgtest_main -lpthread $(FLAGS)
# 	./radix_test $(IN_PLAIN_TEXT_FILE)

compress-int: compressor main clean
	./main $(IN_PLAIN_TEXT_FILE) $(COMPRESSED_FILE) e

decompress-int: main clean
	./main $(COMPRESSED_FILE) $(OUT_PLAIN_TEXT_FILE) d
	diff $(OUT_PLAIN_TEXT_FILE) $(IN_PLAIN_TEXT_FILE)

compress-elias: clean
	$(CC) -c compressor-elias.cpp -o compressor-elias.o $(FLAGS) $(LIBS)
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main compressor-elias.o main.o  $(FLAGS) $(LIBS)
	./main $(IN_PLAIN_TEXT_FILE) $(COMPRESSED_FILE) e

decompress-elias: main clean
	$(CC) -c compressor-elias.cpp -o compressor-elias.o $(FLAGS) $(LIBS)
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main compressor-elias.o main.o  $(FLAGS) $(LIBS)
	./main $(COMPRESSED_FILE) $(OUT_PLAIN_TEXT_FILE) d
	diff $(OUT_PLAIN_TEXT_FILE) $(IN_PLAIN_TEXT_FILE)

clean:
	rm -rf *.o