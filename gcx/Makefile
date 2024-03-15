CC=g++

FLAGS_DEBUG=-g -fsanitize=address -Wall
FLAGS=-std=c++14 -I../external/malloc_count -O3
LIBS=-ldl
MACROS:= D_EXTRACT=0 DEBUG_RULES=0 LEVEL_REPORT=0 SCREEN_OUTPUT=1 FILE_OUTPUT=1

all: run_compressor

fraenkel: ../../fraenkel
	./../../fraenkel 25 $(DIR)/$(FILE)

compile: utils.cpp utils.hpp compressor.cpp compressor.hpp
	$(CC) -c ../external/malloc_count/malloc_count.c -o malloc_count.o $(FLAGS) $(LIBS)
	$(CC) -c ../external/malloc_count/stack_count.c -o stack_count.o $(FLAGS) $(LIBS)
	$(CC) -c uarray.c -o uarray.o $(FLAGS)
	$(CC) -c utils.cpp -o utils.o $(addprefix -D , $(MACROS)) $(FLAGS)
	$(CC) -c compressor.cpp -o compressor.o $(addprefix -D , $(MACROS)) $(FLAGS)
	$(CC) main.cpp malloc_count.o stack_count.o uarray.o utils.o compressor.o -o main $(addprefix -D , $(MACROS)) $(FLAGS)

run_compressor: compile
	@if [ ! -x "main" ]; then\
		$(MAKE) compile;\
	fi
ifeq ($(MODE), -d)
	./main -d $(FILE_IN) $(FILE_OUT)
	diff $(FILE_OUT) $(ORIGINAL) 
else ifeq ($(MODE), -c)
	./main -c $(FILE_IN) $(FILE_OUT)
else ifeq ($(MODE), -e)
	./main -e $(FILE_IN) $(FILE_OUT) $(QUERIES)
	 python3 ../scripts/extract.py $(ORIGINAL) extract_temp.txt $(QUERIES)
	 diff extract_temp.txt $(FILE_OUT)
	 rm extract_temp.txt $(FILE_OUT)
endif

clean:
	rm -rf *.o main
ifeq ($(shell uname -s),Darwin)
	if [ -d "main.dSYM" ]; then\
		rm -R main.dSYM;\
	fi
endif