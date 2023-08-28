CC=g++

FLAGS_DEBUG=-g -fsanitize=address -Wall
FLAGS=-std=c++14 -O3 -I../external/malloc_count

LIBS=-ldl
MACROS:= D_EXTRACT=0 DEBUG_RULES=0 REPORT=0 LEVEL_REPORT=0

all: fraenkel

fraenkel: ../../fraenkel
	./../../fraenkel 25 $(DIR)/$(FILE)

compile: compressor.cpp compressor.hpp compressor-int.cpp compressor-int.hpp
	$(CC) -c ../external/malloc_count/malloc_count.c -o malloc_count.o $(FLAGS) $(LIBS)
	$(CC) -c ../external/malloc_count/stack_count.c -o stack_count.o $(FLAGS) $(LIBS)
	$(CC) -c uarray.c -o uarray.o $(FLAGS)
	$(CC) -c compressor.cpp -o compressor.o $(FLAGS) 
	$(CC) -c compressor-int.cpp -o compressor-int.o $(FLAGS)
	$(CC) main.cpp malloc_count.o stack_count.o uarray.o compressor.o compressor-int.o -o main  $(FLAGS)

run_compressor:
	@if [ ! -x "main" ]; then\
		$(MAKE) compile;\
	fi
ifeq ($(MODE), d)
	./main $(FILE_IN) $(FILE_OUT) d $(COVERAGE)
	cmp $(FILE_OUT) $(ORIGINAL) 
else ifeq ($(MODE), c)
	./main $(FILE_IN) $(FILE_OUT) c $(COVERAGE)
else ifeq ($(MODE), e)
	./main $(FILE_IN) $(FILE_OUT) e $(COVERAGE) $(l) $(r)
	python3 ../utils/extract.py $(ORIGINAL) extract_temp.txt $(l) $(r)
	diff extract_temp.txt $(FILE_OUT)
	rm extract_temp.txt
endif

clean:
	rm -rf *.o main