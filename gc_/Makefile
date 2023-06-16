CC=g++
FLAGS=-Wall -g -std=c++14
LIBS=-I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64

WORST_ENTRY=ABACABADABACABAEABACABADABACABA
DNA_STRING=AGTTTTCATTCTGACTGAACAGCTTTTCATTCTGACTGCAAC
STRING=banaananaanana
TEXT=$(STRING)
NUMBER_OF_PERMUTATIONS=0

DIR=../dataset/pizza_chilli
FILE=cere
IN_PLAIN_TEXT_FILE=$(DIR)/$(FILE)

ifeq ($(CODEC),elias)
	FILE_CPP=compressor-elias
	COMPRESSED_FILE=text/$(FILE)-rules$(RULES_SIZE)-cod-elias
	OUT_PLAIN_TEXT_FILE=text/$(FILE)-plain-elias
else
	FILE_CPP=compressor-int
	COMPRESSED_FILE=text/$(FILE)-rules$(RULES_SIZE)-cod-int
	OUT_PLAIN_TEXT_FILE=text/$(FILE)-plain-int
endif

all: fraenkel

fraenkel: ../../fraenkel
	./../../fraenkel 25 $(IN_PLAIN_TEXT_FILE)

create_text: ../generate_text.cpp
	$(CC) ../generate_text.cpp -o ../generate_text
	.././generate_text $(TEXT) $(NUMBER_OF_PERMUTATIONS) text/in-plain-text.txt

compressor: $(FILE_CPP).cpp $(FILE_CPP).hpp compressor.cpp compressor.hpp main.cpp
	$(CC) -c compressor.cpp -o compressor.o $(FLAGS) $(LIBS) 
	$(CC) -D DEBUG_RULES=0 -D REPORT=1 -D LEVEL_REPORT=1 -c $(FILE_CPP).cpp -o $(FILE_CPP).o $(FLAGS) $(LIBS) 
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main compressor.o $(FILE_CPP).o main.o  $(FLAGS) $(LIBS)
ifeq ($(MODE), d)
	./main $(COMPRESSED_FILE) $(OUT_PLAIN_TEXT_FILE) d $(RULES_SIZE)
	cmp $(OUT_PLAIN_TEXT_FILE) $(IN_PLAIN_TEXT_FILE) 
else
	./main $(IN_PLAIN_TEXT_FILE) $(COMPRESSED_FILE) e $(RULES_SIZE)
endif

compressor-2: compressor-variable-types.cpp compressor-variable-types.hpp compressor.cpp compressor.hpp main.cpp
	$(CC) -c compressor.cpp -o compressor.o $(FLAGS) $(LIBS) 
	$(CC) -D DEBUG_RULES=1 -c compressor-variable-types.cpp -o compressor-variable-types.o $(FLAGS) $(LIBS) 
	$(CC) -c main-2.cpp -o main-2.o  $(FLAGS)
	$(CC) -o main-2 compressor.o compressor-variable-types.o main-2.o  $(FLAGS) $(LIBS)
ifeq ($(MODE), d)
	./main-2 text/$(FILE)-rules$(RULES_SIZE)-variable-types text/$(FILE)-rules$(RULES_SIZE)-variable-types-PLAIN d $(RULES_SIZE)
	diff text/$(FILE)-rules$(RULES_SIZE)-variable-types-PLAIN $(IN_PLAIN_TEXT_FILE) 
else
	./main-2 $(IN_PLAIN_TEXT_FILE) text/$(FILE)-rules$(RULES_SIZE)-variable-types e $(RULES_SIZE)
endif

clean:
	rm -rf *.o main