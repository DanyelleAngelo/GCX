CC=g++

FLAGS_DEBUG=-g -fsanitize=address -Wall
FLAGS=-std=c++14 -I../external/malloc_count -O3 $(FLAGS_DEBUG)
LIBS=-ldl
MACROS:= D_EXTRACT=0 DEBUG_RULES=0 LEVEL_REPORT=0 SCREEN_OUTPUT=1 FILE_OUTPUT=1

OUTPUT ?= ./main

all: fraenkel

fraenkel: ../../fraenkel
	./../../fraenkel 25 $(DIR)/$(FILE)

compile: utils.cpp utils.hpp compressor.cpp compressor.hpp
	$(CC) -c ../external/malloc_count/malloc_count.c -o malloc_count.o $(FLAGS) $(LIBS)
	$(CC) -c ../external/malloc_count/stack_count.c -o stack_count.o $(FLAGS) $(LIBS)
	$(CC) -c uarray.c -o uarray.o $(FLAGS)
	$(CC) -c utils.cpp -o utils.o $(addprefix -D , $(MACROS)) $(FLAGS)
	$(CC) -c compressor.cpp -o compressor.o $(addprefix -D , $(MACROS)) $(FLAGS)
	$(CC) main.cpp malloc_count.o stack_count.o uarray.o utils.o compressor.o -o $(OUTPUT) $(addprefix -D , $(MACROS)) $(FLAGS)

run_compressor:
	@if [ ! -x "main" ]; then\
		$(MAKE) compile;\
	fi
ifeq ($(MODE), d)
	$(OUTPUT) $(FILE_IN) $(FILE_OUT) d $(COVERAGE)
	diff $(FILE_OUT) $(ORIGINAL)
else ifeq ($(MODE), c)
	$(OUTPUT) $(FILE_IN) $(FILE_OUT) c $(COVERAGE)
else ifeq ($(MODE), e)
	$(OUTPUT) $(FILE_IN) $(FILE_OUT) e $(COVERAGE) $(QUERIES)
	python3 ../scripts/extract.py $(ORIGINAL) extract_temp.txt $(QUERIES)
	diff extract_temp.txt $(FILE_OUT)
	rm extract_temp.txt $(FILE_OUT)
endif

clean:
	rm -rf *.o $(OUTPUT)
ifeq ($(shell uname -s),Darwin)
	if [ -d "$(OUTPUT).dSYM" ]; then\
		rm -R $(OUTPUT).dSYM;\
	fi
endif