#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "malloc_count.h"
#include "stack_count.h"
#include "compressor-int.hpp"
#include "compressor.hpp"

using namespace std;

void generateReport(char *fileName, double duration, void *base) {
    FILE *file = fopen(fileName, "a");
    isFileOpen(file, "Unable to open report to enter time and memory consumption informatio");

    long long int peak = malloc_count_peak();
    long long int stack = stack_count_usage(base);

    fprintf(file, "%lld,%lld,%5.2lf,", peak,stack,duration);
    fclose(file);
}

int main(int argc, char *argv[]) {
    if(argc < 5) {
        cout << "\n\x1b[31m[ERROR]\x1b[0m  Number of invalid arguments! \n" <<
                "\tList of arguments:\n" <<
                "\t\tInput file  for encode or decode\n" <<
                "\t\tOutput file, contains the result of the chosen operation)\n" <<
                "\t\tOperation - for compress choose: \"c\", for decode choose: \"d\" \n" <<
                "\t\tSize of rules\n"  << endl;
        exit(EXIT_FAILURE);
    }

    clock_t start, finish;
    char op = argv[3][0];
    int ruleSize = atoi(&argv[4][0]);
    void* base = stack_count_clear();

    start = clock();
    grammarInteger(argv[1], argv[2], op, ruleSize);
    finish = clock();

    double duration = (double)(finish - start) / CLOCKS_PER_SEC;

    if(argc == 6)generateReport(argv[5], duration, base);
    
    printf("Time: %5.2lf(s)\n",duration);
}