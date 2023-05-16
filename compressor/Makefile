CC=g++
FLAGS=-Wall -g -std=c++14
LIBS=-I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

WORST_ENTRY=ABACABADABACABAEABACABADABACABA
DNA_STRING=AGTTTTCATTCTGACTGAACAGCTTTTCATTCTGACTGCAAC
STRING=banaananaanana
TEXT=$(DNA_STRING)
NUMBER_OF_PERMUTATIONS=0

DIR=../dataset/pizza_chilli
FILE=dna.50MB
IN_PLAIN_TEXT_FILE=$(DIR)/$(FILE)

ifeq ($(CODEC),elias)
	FILE_CPP=compressor-elias
	COMPRESSED_FILE=text/$(FILE)-rules$(RULES_SIZE)-cod-elias
	OUT_PLAIN_TEXT_FILE=text/$(FILE)-rules$(RULES_SIZE)-plain-elias
else
	FILE_CPP=compressor-int
	COMPRESSED_FILE=text/$(FILE)-rules$(RULES_SIZE)-cod-int
	OUT_PLAIN_TEXT_FILE=text/$(FILE)-rules$(RULES_SIZE)-plain-int
endif

all: fraenkel

fraenkel: ../../fraenkel
	./../../fraenkel 20 $(IN_PLAIN_TEXT_FILE)

create_text: ../generate_text.cpp
	$(CC) ../generate_text.cpp -o ../generate_text
	.././generate_text $(TEXT) $(NUMBER_OF_PERMUTATIONS) text/in-plain-text.txt

main: $(FILE_CPP).cpp compressor-int.hpp main.cpp
	$(CC) -c $(FILE_CPP).cpp -o $(FILE_CPP).o $(FLAGS) $(LIBS) 
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main $(FILE_CPP).o main.o  $(FLAGS) $(LIBS)
ifeq ($(MODE), d)
	./main $(COMPRESSED_FILE) $(OUT_PLAIN_TEXT_FILE) d $(RULES_SIZE)
	diff $(OUT_PLAIN_TEXT_FILE) $(IN_PLAIN_TEXT_FILE) 
else
	./main $(IN_PLAIN_TEXT_FILE) $(COMPRESSED_FILE) e $(RULES_SIZE)
endif

clean:
	rm -rf *.o