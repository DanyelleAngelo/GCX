CC=g++
FLAGS=-Wall -std=c++14 -g -fsanitize=address
LIBS=-I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64
MACROS:= DEBUG_RULES=0 REPORT=0 LEVEL_REPORT=0

DIR=../dataset/pizza_chilli/real
IN_PLAIN_TEXT_FILE=$(DIR)/$(FILE)
COMPRESSED_FILE=text/$(FILE)-r$(RULES)-$(CODEC)
OUT_PLAIN_TEXT_FILE=text/$(FILE)-PLAIN-$(CODEC)

all: fraenkel

fraenkel: ../../fraenkel
	./../../fraenkel 25 $(DIR)/$(FILE)

compile: compressor.cpp compressor.hpp compressor-$(CODEC).cpp compressor-$(CODEC).hpp
	$(CC) -c compressor.cpp -o compressor.o $(addprefix -D, $(MACROS)) $(FLAGS) $(LIBS) 
	$(CC) -c compressor-$(CODEC).cpp -o compressor-$(CODEC).o $(addprefix -D, $(MACROS)) $(FLAGS) $(LIBS)
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main compressor.o compressor-$(CODEC).o main.o  $(FLAGS) $(LIBS)

run_compressor: 
ifneq ($(wildcard "main"), )
	$(MAKE) compile
endif
ifeq ($(MODE), d)
	./main $(COMPRESSED_FILE) $(OUT_PLAIN_TEXT_FILE) d $(RULES) $(CODEC)
	cmp $(OUT_PLAIN_TEXT_FILE) $(IN_PLAIN_TEXT_FILE) 
else
	./main $(IN_PLAIN_TEXT_FILE) $(COMPRESSED_FILE) e $(RULES) $(CODEC)
endif

clean:
	rm -rf *.o main