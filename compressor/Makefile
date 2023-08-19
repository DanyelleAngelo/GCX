CC=g++
FLAGS=-Wall -std=c++14 -g -fsanitize=address
LIBS=-I ~/include -L ~/lib -lsdsl -ldivsufsort -ldivsufsort64
MACROS:= DEBUG_RULES=0 REPORT=0 LEVEL_REPORT=0

DIR=../dataset/pizza_chilli/real
IN_PLAIN_TEXT_FILE=$(DIR)/$(FILE)
COMPRESSED_FILE=text/$(FILE)-r$(RULES)-int
OUT_PLAIN_TEXT_FILE=text/$(FILE)-PLAIN-int

all: fraenkel

fraenkel: ../../fraenkel
	./../../fraenkel 25 $(DIR)/$(FILE)

compile: compressor.cpp compressor.hpp compressor-int.cpp compressor-int.hpp
	$(CC) -c uarray.c -o uarray.o $(FLAGS)
	$(CC) -c compressor.cpp -o compressor.o $(addprefix -D, $(MACROS)) $(FLAGS) 
	$(CC) -c compressor-int.cpp -o compressor-int.o $(addprefix -D, $(MACROS)) $(FLAGS)
	$(CC) -c main.cpp -o main.o  $(FLAGS)
	$(CC) -o main uarray.o compressor.o compressor-int.o main.o  $(FLAGS)

run_compressor:
	@if [ ! -x "main" ]; then\
		$(MAKE) compile;\
	fi
ifeq ($(MODE), d)
	./main $(COMPRESSED_FILE) $(OUT_PLAIN_TEXT_FILE) d $(RULES)
	cmp $(OUT_PLAIN_TEXT_FILE) $(IN_PLAIN_TEXT_FILE) 
ifeq ($(MODE), c)
	./main $(IN_PLAIN_TEXT_FILE) $(COMPRESSED_FILE) c $(RULES)
else
	./main $(COMPRESSED_FILE) $(COMPRESSED_FILE) e $(RULES)
endif

clean:
	rm -rf *.o main